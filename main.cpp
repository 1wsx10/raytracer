#include "main.h"

int main() {
	v3d v1(1,0,0);
	v3d v2(0,1,0);

	double angle = v3d::angle(v1, v2);

	printf("angle: %f\n", angle);
	return EXIT_SUCCESS;
}
