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
		double& hitdist, int& hitindex,
		v3d& hitloc, v3d& hitnormal) {

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

			if(dist < 1e-6 || dist > hitdist) continue;

			type = HIT::sphere;
			hitindex = i;
			hitdist = dist;
			hitloc = eye + (dir * hitdist);
			hitnormal = (hitloc - spheres[i].center) * (1/r);
		}
	}

	// planes
	{
		for(unsigned i = 0; i <  num_planes; i++) {
			double dv = -planes[i].normal.dot(dir);
			if(dv > -1e-6) continue;
			
			double d2 = planes[i].normal.dot(eye);
			double dist = (d2 + planes[i].offset) / dv;
			if(dist < 1e-6 || dist >= hitdist) continue;

			type = HIT::plane; hitindex = i;
			hitdist = dist;
			hitloc = eye + (dir * hitdist);
			hitnormal = -planes[i].normal;
		}
	}

	return type;
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






#define uint unsigned int

int main() {

	std::unique_ptr<framebuf> fb = framebuf::make_unique();
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

#if 1
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
//complaints about compound literals
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
				colour = (RGBT){.r=(int)(x*255/W), .g=(int)(y*255/H), .b=(int)(x*y*255/(W*H)), .t=255};
#pragma GCC diagnostic pop
			} else {
				assert(false);// add the shape type to the draw call
			}

			pixel_ pix(x, y, colour);

			//draw(fb.get(), std::make_unique<pixel_>(x, y, colour).get());
			draw(fb.get(), &pix);
		}
	}
#endif


	return EXIT_SUCCESS;
}


