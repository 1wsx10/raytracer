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


#define DEBUG 1
#define IS_APPROX_0(X, DELTA) (abs(X) < (DELTA))

