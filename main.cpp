#include "main.h"

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









#define uint unsigned int

int main() {

	std::unique_ptr<framebuf> fb = framebuf::make_unique();
	pixel_ pixel(0, 50, (RGBT){.r=255, .g=0, .b=0, .t=0});

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
				colour = (RGBT){.r=(int)(x*255/W), .g=(int)(y*255/H), .b=(int)(x*y*255/(W*H)), .t=255};
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


