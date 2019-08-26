#ifndef MAIN_H
#define MAIN_H

#include <math.h>
#include <assert.h>
#include <memory>

#include "lib/vector/vector.h"
#include "libdraw.hpp"


namespace HIT {
	enum type {
		undef = -1,
		plane = 0,
		sphere = 1,
	};
}



class matrix {
	public:
		v3d m[3];

		void init_rotate(const v3d& angle) {
			double cx = cos(angle.x), cy = cos(angle.y), cz = cos(angle.z);
			double sx = sin(angle.x), sy = sin(angle.y), sz = sin(angle.z);
			double sxsz = sx*sz, cxsz = cx*sz;
			double cxcz = cx*cz, sxcz = sx*cz;
			matrix result = (matrix){.m={
				v3d(cy*cz, cy*sz, -sy),
				v3d(sxcz*sy - cxsz, sxsz*sy + cxcz, sx*cy),
				v3d(cxcz*sy + sxsz, cxsz*sy - sxcz, cx*cy)
			}};
			*this = result;
		}

		void transform(v3d& vec) {
			vec.set(
					v3d::dot(m[0], vec),
					v3d::dot(m[1], vec),
					v3d::dot(m[2], vec)
					);
		}
};




#endif
