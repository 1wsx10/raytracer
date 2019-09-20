#include "../matrix.hpp"
#include <cstdio>


#define BOOST_TEST_MODULE Matrix test
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_44_times_scalar_rvalue )
{
	bool passed = true;

	m44d five = m44d::unit * 5;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++) {
			if(i == j) {
				// should be 5
				BOOST_CHECK(five[i][j] == 5);
			} else {
				// should be 0
				BOOST_CHECK(five[i][j] == 0);
			}
		}

	BOOST_CHECK(passed);
}

BOOST_AUTO_TEST_CASE(test_44_times_equals_scalar)
{
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

	BOOST_CHECK(passed);
}


BOOST_AUTO_TEST_CASE(test_44_transpose) {
	m44d val =
	{{{1,2,3,4},
		 {5,6,7,8},
		 {9,10,11,12},
		 {13,14,15,16}}};

	m44d should_equal = 
	{{{1,5,9,13},
		 {2,6,10,14},
		 {3,7,11,15},
		 {4,8,12,16}}};

	BOOST_CHECK(m44d::transposition(val) == should_equal);

	val.transpose();
	BOOST_CHECK(val == should_equal);
}
