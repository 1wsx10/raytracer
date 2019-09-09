#pragma once

#include "libdraw.hpp"
#include "lib/write_screen/drawlib.h"
#include "lib/vector/vector.h"
#include "timer.hpp"

#include <cassert>
#include <climits>
#include <cfloat>


#define DEBUG 1
#define IS_APPROX_0(X, DELTA) (abs(X) < (DELTA))

