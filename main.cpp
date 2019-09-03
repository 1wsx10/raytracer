#include "main.h"
#include "timer.hpp"
#include "limits.h"

/* Declarations for Scene description.
 */

/**
 * Walls are Planes. Planes have a normal vector and a Distance.
 */
struct plane {
	v3d normal;
	double offset;
	RGBT colour;
};

struct sphere {
	v3d center;
	double radius;
	RGBT colour;
};

struct light_source {
	v3d position;
	RGBT colour;
};

//complaints about compound literals
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
const struct plane planes[] = {
	/* declare 6 planes, each looks towords origin and is 30 units away
	 */
	{v3d(0,0,-1), -30, (RGBT){.r=128, .g=128, .b=128, .t=255}},
	{v3d(0, 1,0), -30, (RGBT){.r=128, .g=128, .b=128, .t=255}},
	{v3d(0,-1,0), -30, (RGBT){.r=128, .g=128, .b=128, .t=255}},
	{v3d( 1,0,0), -30, (RGBT){.r=128, .g=128, .b=128, .t=255}},
	{v3d(0,0, 1), -30, (RGBT){.r=128, .g=128, .b=128, .t=255}},
	{v3d(-1,0,0), -30, (RGBT){.r=128, .g=128, .b=128, .t=255}},
};

#define RANDOM_POSITION v3d((rand()%60)-30,(rand()%60)-30,(rand()%60)-30)

const struct sphere spheres[] = {
	/* declare a few spheres */
	{RANDOM_POSITION, (double)(rand()%8), (RGBT){.r=128, .g=128, .b=128, .t=255}},
	{RANDOM_POSITION, (double)(rand()%8), (RGBT){.r=128, .g=128, .b=128, .t=255}},
	{RANDOM_POSITION, (double)(rand()%8), (RGBT){.r=128, .g=128, .b=128, .t=255}},
	{RANDOM_POSITION, (double)(rand()%8), (RGBT){.r=128, .g=128, .b=128, .t=255}},
	{RANDOM_POSITION, (double)(rand()%8), (RGBT){.r=128, .g=128, .b=128, .t=255}},
	{RANDOM_POSITION, (double)(rand()%8), (RGBT){.r=128, .g=128, .b=128, .t=255}},
};

const struct light_source lights[] = {
	/* declare some lightsources
	 * a light source has a position and a colour
	 */
	{RANDOM_POSITION, (RGBT){255,255,255,255}},
	{RANDOM_POSITION, (RGBT){255,255,255,255}},
};
#pragma GCC diagnostic pop


#define N_ELEMS(x) sizeof(x) / sizeof(*x)
const unsigned
num_planes = N_ELEMS(planes),
num_spheres = N_ELEMS(spheres),
num_lights = N_ELEMS(lights),
max_trace = 6; // maximuum trace level

/* Actual Raytracing! */
/**********************/
/**
 * Determine weather an object is in direct eyesight on the given
 * line, and determine exactly which point of the object is seen.
 */
HIT::type ray_find_obstacle(const v3d& eye, const v3d& dir,
		double& hit_dist, int& hit_index,
		v3d& hit_loc, v3d& hit_normal) {

	if(!(rand() % 100)) {// check this is normalized every now and then
		assert(fabs(dir.length()) - 1 < 1e-6);
	}

	// Try intersecting the ray with each object and see which one
	// produces the closest result
	HIT::type type = HIT::undef;

	// spheres
	{
		for(unsigned i = 0; i < num_spheres; i++) {
			v3d v (eye - spheres[i].center);
			double r = spheres[i].radius;
			double dv = dir.dot(v);
			//TODO: find out what the squared method should be
			double d2 = dir.length_squared();// i think its length
			double sq = dv * dv - d2 * (v.length_squared() - r * r);

			// does the ray coincide with the spheres?
			if(sq < 1e-6) continue;
			// where exactly?
			double sqt = sqrt(sq);
			double dist = fmin(-dv - sqt, -dv + sqt) / d2;

			if(dist < 1e-6 || dist > hit_dist) continue;

			type = HIT::sphere;
			hit_index = i;
			hit_dist = dist;
			hit_loc = eye + (dir * hit_dist);
			hit_normal = (hit_loc - spheres[i].center) * (1/r);
		}
	}

	// planes
	{
		for(unsigned i = 0; i <  num_planes; i++) {
			double dv = -planes[i].normal.dot(dir);
			if(dv > -1e-6) continue;
			
			double d2 = planes[i].normal.dot(eye);
			double dist = (d2 + planes[i].offset) / dv;
			if(dist < 1e-6 || dist >= hit_dist) continue;

			type = HIT::plane; hit_index = i;
			hit_dist = dist;
			hit_loc = eye + (dir * hit_dist);
			hit_normal = -planes[i].normal;
		}
	}

	return type;
}

const unsigned num_areaa_light_vectors = 20;
v3d area_light_vectors[num_areaa_light_vectors];
void init_area_light_vectors() {
	for(unsigned i = 0; i < num_areaa_light_vectors; i++) {
		// specify a point within 1 unit of the point in any direction
		area_light_vectors[i].x = 2.0 * (rand() / double(RAND_MAX) - 0.5);
		area_light_vectors[i].y = 2.0 * (rand() / double(RAND_MAX) - 0.5);
		area_light_vectors[i].z = 2.0 * (rand() / double(RAND_MAX) - 0.5);
	}
}

/**
 * shoot a camera-ray from the specified location to
 * specified location, and determine the RGB colour of the perception
 * corresponding to that location.
 *
 * @param k    recursion depth
 */
void ray_trace(v3d &result_colour, const v3d &eye, const v3d& dir, int k) {
	double hit_dist = 1e6;
	v3d hit_loc, hit_normal;
	int hit_index;
	HIT::type type = ray_find_obstacle(eye, dir, hit_dist, hit_index, hit_loc, hit_normal);
	if(type != HIT::undef) {
		/* Found an obstacle. Next, find out how it is illuminated.
		 * shoot a ray to each lightsource, and determine if there
		 * is an obstacle behind it. This is called "diffuse light"
		 */
		v3d diffuse_light = v3d(0,0,0);
		v3d specular_light = v3d(0,0,0);
		v3d pigment = v3d(1, 0.98, 0.94);
		for(unsigned i = 0; i < num_lights; i++)
			for(unsigned j = 0; j < num_areaa_light_vectors; j++) {
				v3d v = (lights[i].position + area_light_vectors[j]) - hit_loc;
				v.normalise();

				double light_dist = v.length();
				double diffuse_effect = hit_normal.dot(v) 
					/ (double)num_areaa_light_vectors;

				double attenuation = (1 + pow(light_dist / 34.0, 2.0));
				diffuse_effect /= attenuation;

				if(diffuse_effect > 1e-3) {
					double shadow_dist = light_dist - 1e-4;
					v3d a,b;
					int q;
					HIT::type occluder = ray_find_obstacle(hit_loc + v*1e-4, v, shadow_dist, q, a, b);//TODO: check a, b, and q are in the right positions
						if(occluder == HIT::undef) {
							diffuse_light.x += lights[i].colour.r * diffuse_effect;
							diffuse_light.y += lights[i].colour.g * diffuse_effect;
							diffuse_light.z += lights[i].colour.b * diffuse_effect;
						}
				}
			}

		if(k > 1) {
			// add specular light reflection, unless recursion depth is set at max
			v3d v = -dir; v.mirror(hit_normal);
			ray_trace(specular_light, hit_loc + v*1e-4, v, k-1);
		}
		switch(type) {
			case HIT::plane:
				diffuse_light *= 0.9;
				specular_light *= 0.5;
				// colour the different walls differently
				switch(hit_index % 3) {
					case 0: pigment.set(0.9, 0.7, 0.6); break;
					case 1: pigment.set(0.6, 0.7, 0.7); break;
					case 2: pigment.set(0.5, 0.8, 0.3); break;
				}
				break;
			case HIT::sphere:
				diffuse_light *= 1.0;
				specular_light *= 0.34;
				break;
			case HIT::undef: break;// this will never happen
		}

		result_colour = (diffuse_light + specular_light).mul_elems(pigment);
	}
}


#define LOOP_ASM_TEST 0
	/*  =============================
	 *     testing different loops
	 *  =============================
	 */
#if LOOP_ASM_TEST
int nested_loop(int a, int b) {
	volatile int last = 0;

	for(int i = 0; i < a; i++) {
		for(int j = 0; j < b; j++) {
			last = i * j;
		}
	}

	return last;
}

int single_loop_div(int a, int b) {
	volatile int last = 0;

	for(int x = 0; x < a*b; x++) {
		div_t intermediate = div(x, b);
		int i = intermediate.quot;
		int j = intermediate.rem;

		last = i * j;
	}

	return last;
}

int single_loop_asm(int a, int b) {
#if defined(__i386__) || defined(__x86_64__)
	volatile int last = 0;

	for(int x = 0; x < a*b; x++) {
		// preset i to x
		//   (it becomes the least significant bits of the division)
		//   it then becomes the quotient
		int i = x;
		// preset j to 0
		//   (it becomes the most significant bits of the division)
		//   it then becomes the remainder
		int j = 0;

		/*
		 * idiv: divisor is a 64 bit int (so needs 2 registers)
		 *   most  significant 32 is the EDX register
		 *   least significant 32 is the EAX register
		 *
		 *   EAX register becomes the quotient
		 *   EDX register becomes the remainder
		 */
		__asm__ (
				"idiv %2          \n\t"       //divide by (b)
				:"+a"(i), "+d"(j)             //output
				:"r"(static_cast<int32_t>(b)) //input
				:                             //clobber
				);

		last = i * j;
	}

	return last;
#else
#error
	return single_loop(a, b);
#endif
}

int single_loop(int a, int b) {
	volatile int last = 0;

	for(int x = 0; x < a*b; x++) {
		int i = x / b;
		int j = x % b;

		last = i * j;
	}

	return last;
}
#endif







int main() {

	std::unique_ptr<framebuf, framebuf::deleter> fb = framebuf::make_unique();
	init_area_light_vectors();

//complaints about compound literals
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	pixel_ pixel(0, 50, (RGBT){.r=255, .g=0, .b=0, .t=0});
#pragma GCC diagnostic pop

#if 0
	int i;
	((PIXEL*)&pixel)->x = &i; // this compiles to the same thing as changing the value of a FBINFO
	for(i = 30; i < 100; i++) {
		pixel.x = i;
		draw(fb.get(), &pixel);
	}
#endif

	const unsigned W = fb->vinfo.xres;
	const unsigned H = fb->vinfo.yres;

	/*
	for(unsigned frameno = 0; frameno < 9300; ++frameno) {
		for(unsigned y = 0; y < H; y++) {
			for(unsigned x = 0; x < W; x++) {
				pixel.x = x;
				pixel.y = y;
				//pixel.colour = (RGBT){};
				draw(fb.get(), &pixel);
			}
		}
	}
	*/


	/*  =============================
	 *     testing different loops
	 *  =============================
	 */
#if LOOP_ASM_TEST
	int asmall = 3;
	int bsmall = 8;
	int a = 1010;
	int b = 2327;

	assert((long long int)a*(long long int)b < INT_MAX);

	printf("double loop: \n");
	{ timer timer_double;

		nested_loop(a, b);
	}

	printf("single loop / %%: \n");
	{ timer timer_single;

		single_loop(a, b);
	}

	printf("single loop div: \n");
	{ timer timer_single_div;

		single_loop_div(a, b);
	}

	printf("single loop asm: \n");
	{ timer timer_single_asm;

		single_loop_asm(a, b);
	}
#endif


	v3d eye(0,0,0);
	v3d dir(1,1,0);
	dir.normalise();

	{ timer mytimer;// start timer here

		for(uint x = 0; x < W; x++) {
			for(uint y = 0; y < H; y++) {
				double hitdist = -1;
				int hitindex = -1;
				v3d hitloc;
				v3d hitnormal;
				RGBT colour;

				// try to find an object
				HIT::type type = ray_find_obstacle(eye, dir, hitdist, hitindex, hitloc, hitnormal);

				// look up the shape, so that we know the colour to draw
				if(type == HIT::sphere) {
					colour = spheres[hitindex].colour;
				} else if(type == HIT::plane) {
					colour = planes[hitindex].colour;
				} else if(type == HIT::undef) {
					// give a visual indication of undef
					colour = {(int)(x*255/W), (int)(y*255/H), (int)(x*y*255/(W*H)), 255};
				} else {
					assert(false);// add the shape type to the draw call
				}

				pixel_ pix(x, y, colour);

				//draw(fb.get(), std::make_unique<pixel_>(x, y, colour).get());
				draw(fb.get(), &pix);
			}
		}

	}// timer finishes here

	return EXIT_SUCCESS;
}


