#include "../matrix.hpp"
#include <cstdio>
#include <cmath>


#define BOOST_TEST_MODULE Matrix test
#include <boost/test/unit_test.hpp>



// start tests for m44d


BOOST_AUTO_TEST_CASE(test_44_make_tx)
{
	{// rotate 90 around x
		m44d rotate_90_around_x = m44d::make_transformation(
				v3d::rotate(v3d::X, 90, v3d::X),
				v3d::rotate(v3d::Y, 90, v3d::X),
				v3d::rotate(v3d::Z, 90, v3d::X)
				);
		m44d rotate_90_around_x_precalc =
		{{{1,0,0,0},
			{0,cos(90*PI/180),-sin(90*PI/180),0},
			{0,sin(90*PI/180),cos(90*PI/180),0},
			{0,0,0,1}}};

		BOOST_TEST(rotate_90_around_x == rotate_90_around_x_precalc);

		{
			v3d x = v3d::X;
			m41d xm(x, 1);
			xm = rotate_90_around_x * xm;
			v3d &xv = xm;

			BOOST_TEST(xv.equals(v3d::X, 1e-5), ""<<xv<<".equals("<<v3d::X<<")");
		}
		{
			v3d y = v3d::Y;
			m41d ym(y, 1);
			ym = rotate_90_around_x * ym;
			v3d &zv = ym;

			BOOST_TEST(zv.equals(v3d::Z, 1e-5), ""<<zv<<".equals("<<v3d::Z<<")");
		}
		{
			v3d z = v3d::Z;
			m41d zm(z, 1);
			zm = rotate_90_around_x * zm;
			v3d &yv = zm;

			BOOST_TEST(yv.equals(-v3d::Y, 1e-5), ""<<yv<<".equals("<<-v3d::Y<<")");
		}
	}

	{// scale 3 in x direction
		m44d scale_3_x = m44d::make_transformation(
				v3d::X * 3,
				v3d::Y,
				v3d::Z
				);
		m44d scale_3_x_precalc =
		{{{3,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1}}};

		BOOST_TEST(scale_3_x == scale_3_x_precalc);

		{
			v3d x = v3d::X;
			m41d xm(x, 1);
			xm = scale_3_x * xm;
			v3d &xv = xm;

			BOOST_TEST(xv.equals(v3d::X * 3, 1e-5), ""<<xv<<".equals("<<v3d::X*3<<")");
		}
		{
			v3d y = v3d::Y;
			m41d ym(y, 1);
			ym = scale_3_x * ym;
			v3d &yv = ym;

			BOOST_TEST(yv.equals(v3d::Y, 1e-5), ""<<yv<<".equals("<<v3d::Y<<")");
		}
		{
			v3d z = v3d::Z;
			m41d zm(z, 1);
			zm = scale_3_x * zm;
			v3d &zv = zm;

			BOOST_TEST(zv.equals(v3d::Z, 1e-5), ""<<zv<<".equals("<<v3d::Z<<")");
		}
	}

	{// translate 3 in x direction
		m44d translate_3_x = m44d::make_translation(3 * v3d::X);
		m44d translate_3_x_precalc =
		{{
			{1,0,0,3},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1}}};
		point asdf = v3d(1,22,3);
		v3d& asdf_v = asdf;

		asdf = translate_3_x * asdf;
		std::cout << "asdf: "<<asdf_v<<std::endl;

		BOOST_TEST(translate_3_x == translate_3_x_precalc,
				"translate_3_x = "<< translate_3_x << "\n" <<
				"translate_3_x_precalc = " << translate_3_x_precalc << "\n"
				);

		{
			v3d x = v3d::X;
			m41d xm(x, 1);
			xm = translate_3_x * xm;
			v3d &xv = xm;

			BOOST_TEST(xv.equals(v3d::X * 4, 1e-5), ""<<xv<<".equals("<<v3d::X*4<<")");
		}
		{
			v3d y = v3d::Y;
			m41d ym(y, 1);
			ym = translate_3_x * ym;
			v3d &yv = ym;

			BOOST_TEST(yv.equals(v3d::Y + v3d::X*3, 1e-5), ""<<yv<<".equals("<<v3d::Y + v3d::X * 3<<")");
		}
		{
			v3d z = v3d::Z;
			m41d zm(z, 1);
			zm = translate_3_x * zm;
			v3d &zv = zm;

			BOOST_TEST(zv.equals(v3d::Z + v3d::X*3, 1e-5), ""<<zv<<".equals("<<v3d::Z + v3d::X * 3<<")");
		}
	}




}

BOOST_AUTO_TEST_CASE(test_44_determinant)
{
	BOOST_CHECK(m44d::unit.det() == 1);
}

BOOST_AUTO_TEST_CASE(test_44_inverse)
{
	m44d to_invert =
	{{{3,2,3,4},
		 {5,6,7,8},
		 {9,10,11,12},
		 {13,14,15,16}}};
	m44d copy = to_invert;

	std::cout << to_invert[0][0] << " " << to_invert[0][1] << " " << to_invert[0][2] << " " << to_invert[0][3] << std::endl;

	bool works = to_invert.invert();

	std::cout  << to_invert.det() << "\n\n" << to_invert << std::endl;

	std::cout  << "\n\nshould_unit:\n" << copy*to_invert << std::endl;

	BOOST_CHECK(works);
}

BOOST_AUTO_TEST_CASE(test_44_transpose)
{
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

// matrix arithmetic vs another 4x4
BOOST_AUTO_TEST_CASE(test_44_equality)
{
	m44d val =
	{{{1,2,3,4},
		 {5,6,7,8},
		 {9,10,11,12},
		 {13,14,15,16}}};

	{
		m44d val2 = val;

		BOOST_CHECK(val == val2);
		BOOST_CHECK(!(val != val2));
		bool either_failed = !((val == val2) && (!(val != val2)));
		
		if(either_failed)
			for(int i = 0; i < 4; i++)
				for(int j = 0; j < 4; j++)
					BOOST_CHECK(val[i][j] == val2[i][j]);
	}

	{
		m44d notval =
		{{{16,5,9,13},
			 {2,11,10,14},
			 {3,7,6,15},
			 {4,8,12,1}}};

		BOOST_CHECK(val != notval);
		BOOST_CHECK(!(val == notval));
		bool either_succeeded = !((val != notval) && (!(val == notval)));

		if(either_succeeded)
			for(int i = 0; i < 4; i++)
				for(int j = 0; j < 4; j++)
					BOOST_CHECK(val[i][j] != notval[i][j]);
	}
}

BOOST_AUTO_TEST_CASE(test_44_adition)
{
	m44d a, b, addition, subtraction;
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++) {
			a[i][j] = i + j;
			b[i][j] = PI * i + j*j;
			addition[i][j] = i + j + PI*i + j*j;
			subtraction[i][j] = (i + j) - (PI*i + j*j);
		}

	BOOST_CHECK(a+b == addition);
	BOOST_CHECK(a-b == subtraction);

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++) {
			BOOST_CHECK(a[i][j]+b[i][j] == addition[i][j]);
			BOOST_CHECK((a+b)[i][j] == addition[i][j]);

			BOOST_CHECK(a[i][j]-b[i][j] == subtraction[i][j]);
			BOOST_CHECK((a-b)[i][j] == subtraction[i][j]);
		}
}

BOOST_AUTO_TEST_CASE(test_44_multiplication)
{
	m44d a =
	{{{1,2,3,4},
		 {5,6,7,8},
		 {9,10,11,12},
		 {13,14,15,16}}};

	m44d b =
	{{{2,3,4,5},
		 {6,7,8,9},
		 {10,11,12,13},
		 {14,15,16,17}}};

	m44d a_times_b =
	{{{2+12+30+56,3+14+33+45,4+16+36+64,5+10+39+68},
		 {10+36+70+112,15+42+77+120,20+48+84+128,25+54+91+136},
		 {9*2+10*6+11*10+12*14,9*3+10*7+11*11+12*15,9*4+10*8+11*12+12*16,9*5+10*9+11*13+12*17},
		 {13*2+14*6+15*10+16*14,13*3+14*7+15*11+16*15,13*4+14*8+15*12+16*16,13*5+14*9+15*13+16*17}}};
#if 0
 1, 2, 3, 4
 5, 6, 7, 8
 9,10,11,12
13,14,15,16

 2, 3, 4, 5
 6, 7, 8, 9
10,11,12,13
14,15,16,17
#endif


#if 0
vim macro to get the inside of matrix:
:s/,/,\n€kbr/gkj3k$jjD4j^3j^3kldk$3kA * €ýa$pl€ýa$3jA +€ýa3JJ$hD

set the matrix out like this, put the cursor on the @

@1, 2, 3, 4
 2
 6
10
14
#endif


	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			BOOST_CHECK((a*b)[i][j] = a_times_b[i][j]);

 a *= b;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			BOOST_CHECK((a*b)[i][j] = a_times_b[i][j]);
}

BOOST_AUTO_TEST_CASE(test_44_negation)
{
	m44d val =
	{{{1,2,3,4},
		 {5,6,7,8},
		 {9,10,11,12},
		 {13,14,15,16}}};

	m44d antival =
	{{{-1,-2,-3,-4},
		 {-5,-6,-7,-8},
		 {-9,-10,-11,-12},
		 {-13,-14,-15,-16}}};

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			BOOST_CHECK((-val)[i][j] == antival[i][j]);
}

// scalar arithmatic
BOOST_AUTO_TEST_CASE(test_scalar_times_44)
{
	m44d five = 5 * m44d::unit;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			if(i == j)
				BOOST_CHECK(five[i][j] == 5);
			else
				BOOST_CHECK(five[i][j] == 0);

	// dividion here is undefined
	// number / matrix = ??
}

BOOST_AUTO_TEST_CASE(test_44_times_scalar)
{
	m44d five = m44d::unit * 5;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			if(i == j)
				BOOST_CHECK(five[i][j] == 5);
			else
				BOOST_CHECK(five[i][j] == 0);

	m44d unit = five / 5;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			if(i == j)
				BOOST_CHECK(unit[i][j] == 1);
			else
				BOOST_CHECK(unit[i][j] == 0);
}

BOOST_AUTO_TEST_CASE(test_44_times_equals_scalar)
{
	m44d five = m44d::unit;
	five *= 5;

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

	m44d unit = five;
	unit /= 5;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++) {
			if(i == j) {
				// should be 1
				BOOST_CHECK(unit[i][j] == 1);
			} else {
				// should be 0
				BOOST_CHECK(unit[i][j] == 0);
			}
		}
}

// array ops
BOOST_AUTO_TEST_CASE(test_44_array_conversion)
{
	m44d val = 
	{{{1,2,3,4},
		 {1,2,3,4},
		 {1,2,3,4},
		 {1,2,3,4}}};

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			BOOST_CHECK(&val[i][j] == &val.n[i][j]);

	const m44d cval = val;
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			BOOST_CHECK(&cval[i][j] == &cval.n[i][j]);
}





// end tests for m44d


