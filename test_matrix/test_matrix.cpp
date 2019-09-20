#include "../matrix.hpp"
#include <cstdio>


bool test_44_times_scalar_rvalue() {
	printf("5 * unit:\n");
	bool passed = true;

	m44d five = m44d::unit * 5;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++) {
			if(i == j) {
				// should be 5
				if(five[i][j] != 5) {
					printf("error: m44d::unit * 5 invalid.\n[%d][%d]: %2.2f\n",
							i, j, five[i][j]);
					passed = false;
				}
			} else {
				// should be 0
				if(five[i][j] != 0) {
					printf("error: m44d::unit * 5 invalid.\n[%d][%d]: %2.2f\n",
							i, j, five[i][j]);
					passed = false;
				}
			}
		}
	return passed;
}

bool test_44_times_equals_scalar() {
	printf("5 * unit:\n");
	bool passed = true;

	m44d five = m44d::unit;
	five *= 5;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++) {
			if(i == j) {
				// should be 5
				if(five[i][j] != 5) {
					printf("error: m44d::unit * 5 invalid.\n[%d][%d]: %2.2f\n",
							i, j, five[i][j]);
					passed = false;
				}
			} else {
				// should be 0
				if(five[i][j] != 0) {
					printf("error: m44d::unit * 5 invalid.\n[%d][%d]: %2.2f\n",
							i, j, five[i][j]);
					passed = false;
				}
			}
		}
	return passed;
}

int main() {
	test_44_times_scalar_rvalue();
	test_44_times_equals_scalar();
}
