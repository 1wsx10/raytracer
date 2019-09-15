#include "matrix.hpp"
#include <utility>
#include <cstring>

inline double det2(double a, double b, double c, double d) {
	return a*d - b*c;
}

inline double det3(
		double a, double b, double c,
		double d, double e, double f,
		double g, double h, double i) {
	double ret = 0;

	ret += a * det2(e, f, h, i);
	ret -= b * det2(d, f, g, i);
	ret += c * det2(d, e, g, h);
	return ret;
}

double m44d::det() const {
	//TODO better determinant, LU algorithm
	double det = 0;
	det += n[0][0] * det3(
			n[1][1],n[1][2],n[1][3],
			n[2][1],n[2][2],n[2][3],
			n[3][1],n[3][2],n[3][3]);

	det -= n[0][1] * det3(
			n[1][0],n[1][2],n[1][3],
			n[2][0],n[2][2],n[2][3],
			n[3][0],n[3][2],n[3][3]);

	det += n[0][2] * det3(
			n[1][0],n[1][1],n[1][3],
			n[2][0],n[2][1],n[2][3],
			n[3][0],n[3][1],n[3][3]);

	det -= n[0][3] * det3(
			n[1][0],n[1][1],n[1][2],
			n[2][0],n[2][1],n[2][2],
			n[3][0],n[3][1],n[3][2]);

	return det;
}


bool m44d::invert() {
	double determinant = det();
	if(determinant == 0) {
		return false;
	}

	*this *= 1/determinant;
	return true;
}

bool m44d::inverse(m44d &out) {
	double determinant = out.det();
	if(determinant == 0) {
		return false;
	}
	out = 1/determinant * out;
	return true;
}

m44d& m44d::transpose() {
	/* we are swapping all values that are
	 * not on the leading diagonal, also need
	 * to make sure we don't swap twice
	 *
	 * 00 01 02 03
	 * 10 11 12 13
	 * 20 21 22 23
	 * 30 31 32 33
	 */
	std::swap<double>(n[0][1], n[1][0]);
	std::swap<double>(n[0][2], n[2][0]);
	std::swap<double>(n[0][3], n[3][0]);
	std::swap<double>(n[1][2], n[2][1]);
	std::swap<double>(n[1][3], n[3][1]);
	std::swap<double>(n[2][3], n[3][2]);
	return *this;
}

m44d m44d::transposition(const m44d &other) {
	m44d ret;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			ret.n[j][i] = other.n[i][j];

	return ret;
}

// constants

const m44d m44d::zero = {.n={{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}};
const m44d m44d::unit = {
	.n={
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
}};

// addition

m44d& m44d::operator+=(const m44d &other) {
	for(int i = 0; i < 4*4; i++)
		((double*)n)[i] += ((double*)other.n)[i];
	return *this;
}

m44d m44d::operator+(const m44d &other) const {
	m44d ret = *this;
	ret += other;
	return ret;
}


// subtraction

m44d& m44d::operator-=(const m44d &other) {
	for(int i = 0; i < 4*4; i++)
		((double*)n)[i] -= ((double*)other.n)[i];
	return *this;
}

m44d m44d::operator-(const m44d &other) const {
	m44d ret = *this;
	ret -= other;
	return ret;
}

// negation

m44d m44d::operator-() const {
	return -1 * *this;
}

// multiplication

#include <cstring>
m44d m44d::operator*(const m44d &other) const {
	m44d ret = m44d::zero;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			for(int k = 0; k < 4; k++)
				ret.n[i][j] += n[k][j] * other.n[i][k];

	return ret;
}

m44d& m44d::operator*=(const m44d &other) {
	*this = *this * other;
	return *this;
}


// scalar multiplication

m44d m44d::operator*(double val) const {
	m44d ret = *this;
	ret *= val;
	return ret;
}

m44d m44d::operator/(double val) const {
	m44d ret = *this;
	ret /= val;
	return ret;
}

m44d operator*(double val, const m44d &matrix) {
	return matrix * val;
}
m44d operator/(double val, const m44d &matrix) {
	return matrix * val;
}

m44d& m44d::operator*=(double val) {
	for(int i = 0; i < 4*4; i++)
		((double*)n)[i] *= val;
	return *this;
}

m44d& m44d::operator/=(double val) {
	for(int i = 0; i < 4*4; i++)
		((double*)n)[i] /= val;
	return *this;
}
