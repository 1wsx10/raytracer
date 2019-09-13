#pragma once

#include "libdraw.hpp"
#include "lib/write_screen/drawlib.h"
#include "lib/vector/vector.h"
#include "timer.hpp"

#include <cassert>
#include <climits>
#include <cfloat>

#include <ncurses.h>

#include <chrono>

#include <linux/input.h>
#include <fcntl.h>
#include <cstdlib>

#include <sys/types.h>
#include <regex.h>

#include <pthread.h>
#include "mutex.hpp"


#define DEBUG 1
#define IS_APPROX_0(X, DELTA) (abs(X) < (DELTA))


pthread_mutex_t direction_mutex = PTHREAD_MUTEX_INITIALIZER;
v3d direction(1,0,0);
pthread_mutex_t translation_mutex = PTHREAD_MUTEX_INITIALIZER;
v3d translation(-13, 2, -3);

pthread_mutex_t fov_mutex = PTHREAD_MUTEX_INITIALIZER;
double fov = 90;

//pthread_mutex_t quit_mutex = PTHREAD_MUTEX_INITIALIZER;
mutex quit_mutex("quit flag");
bool quit = false;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-const-variable"
// used in main

const double units_per_sec = 2;
const char arg_onerender[] = "--one_render";

#pragma GCC diagnostic pop




namespace HIT {
	enum type {
		undef = 0,
		sphere = 1,
	};
}

typedef struct {
	v3d c;//centre
	double r;//radius
} sphere;

sphere spheres[] = {
	{v3d(0,0,0), 5},
	{v3d(-4, 2.5, -3), 1},
};
unsigned int num_spheres = sizeof(spheres) / sizeof(spheres[0]);


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
