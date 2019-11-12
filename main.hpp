#pragma once
#include <csignal>

#include "libdraw.hpp"
#include "lib/write_screen/drawlib.h"
#include "lib/vector/vector.h"
#include "timer.hpp"
#include "matrix.hpp"

#include <cassert>
#include <climits>
#include <cfloat>

#include <ncurses.h>
#define GETCH_BLOCK_TIME_MS 5

#include <chrono>

#include <linux/input.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <sys/types.h>
#include <regex.h>

#include <pthread.h>
#include <unistd.h>
#include "mutex.hpp"

#define DEBUG 1
#define IS_APPROX_0(X, DELTA) (abs(X) < (DELTA))


mutex direction_mutex("direction_mutex");
v3d direction(1,0,0);
m44d rotate_tx = m44d::unit;
mutex translation_mutex("translation_mutex");
v3d translation(-13, 2, -3);


mutex fov_mutex("fov_mutex");
double fov = 90;

//mutex quit_mutex("quit_mutex");
mutex quit_mutex("quit flag");
bool quit = false;

struct script_keyframe {
	int frame_num;
	v3d trans;
	v3d dir;
	double fov;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-const-variable"
// used in main

double dolly_zoom_d(double width, double fov) {
	return width /
		(2 * tan(0.5 * (fov * PI / 180)));
}

/*
const v3d pos_0(-13, 2, 0);
const v3d pos_1(-6.5, 2, 0);
*/
const v3d pos_0 = v3d(-1,0,0) * dolly_zoom_d(10, 25);
const v3d pos_1 = v3d(-1,0,0) * dolly_zoom_d(14, 90);
const v3d pos_2(-7, 2, 3);
const v3d pos_3(-2, 1, 12);
const v3d pos_4(10, 2, 0);
const v3d pos_5(6, 2, -6);
const v3d pos_6(0, 2, -10);
const size_t script_len = 7;
// each frame_num should be greater than the last
const script_keyframe script[] = {
	{0, pos_0, v3d(1, 0, 0), 25},//dolly zoom :D
	{15, pos_1, v3d(1, 0, 0), 90},//dolly zoom :D
	{20, pos_2, (v3d::zero - pos_2).normalise(), 90},
	{30, pos_3, (v3d::zero - pos_3).normalise(), 90},
	{35, pos_4, (v3d::zero - pos_4).normalise(), 90},
	{40, pos_5, (v3d::zero - pos_5).normalise(), 90},
	{50, pos_6, (v3d::zero - pos_6).normalise(), 90},
};

const double units_per_sec = 10;
const char arg_onerender[] = "--one_render";
const char arg_scripted[] = "--scripted";

#pragma GCC diagnostic pop

bool curses_enabled = false;



namespace HIT {
	enum type {
		undef = 0,
		sphere,
		plane,
		rect,
	};
}

struct shape {
	RGBT col;//colour
};

struct plane : public shape {
	v3d pos;//position
	v3d dir;//direction

	plane(const RGBT& col, const v3d& pos, const v3d& dir)
		: shape{col}, pos(pos), dir(dir) {};
};

struct rect : plane {
	double w;//width
	double h;//height
	double r;//rotation, radians

	rect(const RGBT& col, const v3d& pos, const v3d& dir,
			double w, double h, double r)
		: plane(col, pos, dir),
		w(w), h(h), r(r) {};
};

struct sphere : public shape {
	v3d c;//centre
	double r;//radius

	sphere(const RGBT& col, const v3d& c, double r)
		: shape{col}, c(c), r(r) {};
};

sphere spheres[] = {
	sphere({0,255,255,255}, v3d(0,0,0), 5),
	sphere({0,255,0,0}, v3d(-4, 2.5, -3), 1),
};
unsigned int num_spheres = sizeof(spheres) / sizeof(spheres[0]);

plane planes[] = {
	plane({0,0,255,0}, v3d(0,-2,0), v3d(0,1,0)),
};
unsigned int num_planes = sizeof(planes) / sizeof(planes[0]);

rect rects[] = {
	rect({255,0,0,0}, v3d(0,0,3), v3d(0,0,-1), 1,1,0),
};
unsigned int num_rects = sizeof(rects) / sizeof(rects[0]);


class curse {
	public:
		struct deleter {
			void operator()(curse* __ptr) const {
				endwin();
			};
			void end(void *__ptr) const {
				endwin();
			};
		};

		WINDOW *window;

		static std::unique_ptr<curse, curse::deleter> make_unique() {
			return std::unique_ptr<curse, deleter>(
					new curse(initscr()), deleter());
		};

		static std::shared_ptr<curse> make_shared() {
			return std::shared_ptr<curse>(new curse(initscr()), deleter());
		};

	protected:
		// don't want anything to call this
		curse(WINDOW *window) :window(window) {};
};
