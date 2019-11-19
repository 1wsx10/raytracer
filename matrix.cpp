#include "matrix.hpp"
#include <utility>
#include <cstring>
#include <cassert>


/** m1d class
 * pure virtual class, for m14d and m41d
 *   (not actually pure virtual)
 * implements conversion to m41d and m14d
 * implements conversion to/from v3d
 */
const m1d m1d::zero = {0,0,0,0};


// 'correct' homogeneous coordinates
template<bool check>
m1d& m1d::correct() {
	if(check && n[3] == 1) {
		return *this;
	}

	*this /= n[3];
	return *this;
}
template<bool check>
m1d m1d::corrected() const {
	if(check && n[3] == 1) {
		return *this;
	}

	return *this / n[3];
}

#if 0
void declare_types() {
	m1d asdf(5,3,2,1);
	asdf.correct<true>();
	asdf.correct<false>();
	asdf.corrected<true>();
	asdf.corrected<false>();
}
#endif

// maths with scalars
m1d m1d::operator*(const double &rhs) const {
	m1d out;
	for(int i = 0; i < 4; i++)
		out[i] = n[i] * rhs;
	return out;
}
m1d& m1d::operator*=(const double &rhs) {
	for(int i = 0; i < 4; i++)
		n[i] *= rhs;
	return *this;
}
m1d m1d::operator/(const double &rhs) const {
	m1d out;
	for(int i = 0; i < 4; i++)
		out[i] = n[i] / rhs;
	return out;
}
m1d& m1d::operator/=(const double &rhs) {
	for(int i = 0; i < 4; i++)
		n[i] /= rhs;
	return *this;
}

// conversions
// to v3d
m1d::operator v3d&() {
	static_assert(v3d::ensure_contiguous_data(),
			"v3d should be contiguous");
	return *reinterpret_cast<v3d*>(this);
}

m1d::operator const v3d&() const {
	static_assert(v3d::ensure_contiguous_data(),
			"v3d should be contiguous");
	return *reinterpret_cast<const v3d*>(this);
}

// convert to m14d
// there is no difference, so we can just alias
m1d::operator m14d&() {
	return static_cast<m14d&>(*this);
}
m1d::operator const m14d&() const {
	return *static_cast<const m14d*>(this);
}
// convert to m41d
m1d::operator m41d&() {
	return static_cast<m41d&>(*this);
}
m1d::operator const m41d&() const {
	return *static_cast<const m41d*>(this);
}



/** m41d class
 *  parent of vector and point
 */

// assignment operator
m41d& m41d::operator=(const m1d &from) {
	for(int i = 0; i < 4; i++)
		n[i] = from[i];
	return *this;
}

// transpose (turn to m14d)
// we can just use the m1d explicit conversion op (which aliases)
m14d& m41d::transposed() {
	return static_cast<m14d&>(*this);
}

const m14d& m41d::transposed() const {
	return static_cast<const m14d&>(*this);
}

// arithmetic with self
m41d m41d::operator+(m41d other) const {
	other += *this;
	return other;
}
m41d m41d::operator-(m41d other) const {
	other -= *this;
	return other;
}
m41d& m41d::operator+=(const m41d &other) {
	for(int i = 0; i < 4; i++)
		n[i] += other[i];
	return *this;
}
m41d& m41d::operator-=(const m41d &other) {
	for(int i = 0; i < 4; i++)
		n[i] -= other[i];
	return *this;
}

m44d m41d::operator*(const m14d& rhs) {
	m44d out;
	/*  [x] * [xxxx] = [xxxx]
	 *  [x]            [xxxx]
	 *  [x]            [xxxx]
	 *  [x]            [xxxx]
	 */

	for(int i = 0; i < 4; i++)     //row in dest
		for(int j = 0; j < 4; j++)   //col in dest
			// k is always 1
			//for(int k = 0; k < 4; k++) //idx of val in current row/col
				out[i][j] = n[i] * rhs[j];

	return out;
}


/** vector class
 *
 */


/** point class
 *
 */


/** m14d class
 *
 */
// assignment op
m14d& m14d::operator=(const m1d &from) {
	for(int i = 0; i < 4; i++)
		n[i] = from[i];
	return *this;
}

// transpose (turn to m41d)
// we can just use the m1d explicit conversion op (which aliases)
m41d& m14d::transposed() {
	return static_cast<m41d&>(*this);
}

const m41d& m14d::transposed() const {
	return static_cast<const m41d&>(*this);
}

// arithmetic with self
m14d m14d::operator+(m14d other) const {
	other += *this;
	return other;
}
m14d m14d::operator-(m14d other) const {
	other -= *this;
	return other;
}
m14d& m14d::operator+=(const m14d &other) {
	for(int i = 0; i < 4; i++)
		n[i] += other[i];
	return *this;
}
m14d& m14d::operator-=(const m14d &other) {
	for(int i = 0; i < 4; i++)
		n[i] -= other[i];
	return *this;
}


// multiply with 4x4
m14d m14d::operator*(const m44d& rhs) const {
	m14d out(m1d::zero);
	/*  [xxxx] * [xxxx] = [xxxx]
	 *           [xxxx]
	 *           [xxxx]
	 *           [xxxx]
	 */
	for(size_t j = 0; j < 4; j++)
		for(size_t k = 0; k < 4; k++)
			out[j] += n[k] * rhs.n[k][j];

	return out;
}

// multiply with 4x1
double m14d::operator*(const m41d& rhs) const {
#if 1
	// simd version
	double out=0;
	for(int i = 0; i < 4; i++)
		out += n[i]*rhs[i];
	return out;
#else
	return
		n[0]*rhs[0] +
		n[1]*rhs[1] +
		n[2]*rhs[2] +
		n[3]*rhs[3];
#endif
}




// checks to see if we got the implicit operators right
#if 0
// these operators should never work
void no_work() {
	// (these ctors should work)
	m44d m4x4 = m44d::unit;
	m41d m4x1(m1d::zero);
	m14d m1x4(m1d::zero);

	// 4x1 * 4x4
	auto result1 = m4x1 * m4x4;

	// 4x4 * 1x4
	auto result2 = m4x4 * m1x4;

	// 4x1 * 4x1
	auto result3 = m4x1 * m4x1;
	
	// 1x4 * 1x4
	auto result4 = m1x4 * m1x4;
}
#endif

#if 0
//conversely, these should
void work() {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
	m44d m4x4 = m44d::unit;
	m14d m1x4(m1d::zero);
	m41d m4x1(m1d::zero);

	m14d r1x4 = m1x4 * m4x4;

	m41d r4x1 = m4x4 * m4x1;

	double r1x1 = m1x4 * m4x1;

	// 4x1 * 1x4
	m4x4 = m4x1 * m1x4;

	m14d from41(m4x1);
	m41d from14(m1x4);

	m14d asdf(m1d(1,2,3,4));
	m41d asdf2(m1d(1,2,3,4));
	asdf = m1d::zero;
	asdf2 = m1d::zero;

	m14d fdsa(m1d::zero);
	m41d fdsa2(m1d::zero);

	m14d fromvec1(v3d::zero, 1);
	
	vector sdfa(v3d::zero);
	point fdsas(v3d::zero);
#pragma GCC diagnostic pop
}
#endif





// constants

const m44d m44d::zero = {.n={{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}};
const m44d m44d::unit = {
	.n={
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
}};

// functions
m44d m44d::make_transformation(
		const v3d& i, const v3d& j, const v3d& k) {
	m44d out = m44d::unit;

	for(int idx = 0; idx < 3; idx++) {
		out.n[0][idx] = i[idx];
		out.n[1][idx] = j[idx];
		out.n[2][idx] = k[idx];
	}

	return out;
}


m44d m44d::make_translation(const v3d& tx) {
	m44d out = m44d::unit;

	for(int i = 0; i < 3; i++) {
		out[i][3] = tx[i];
	}

	return out;
}

// used for m44d::det();
inline double det2(double a, double b, double c, double d) {
	return a*d - b*c;
}

// used for m44d::det();
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

// determinant
double m44d::det() const {
	//TODO better determinant, LU algorithm
	double det = 0;
	det += n[0][0] * det3(
			n[1][1],n[2][1],n[3][1],
			n[1][2],n[2][2],n[3][2],
			n[1][3],n[2][3],n[3][3]);

	det -= n[1][0] * det3(
			n[0][1],n[2][1],n[3][1],
			n[0][2],n[2][2],n[3][2],
			n[0][3],n[2][3],n[3][3]);

	det += n[2][0] * det3(
			n[0][1],n[1][1],n[3][1],
			n[0][2],n[1][2],n[3][2],
			n[0][3],n[1][3],n[3][3]);

	det -= n[3][0] * det3(
			n[0][1],n[1][1],n[2][1],
			n[0][2],n[1][2],n[2][2],
			n[0][3],n[1][3],n[2][3]);

	return det;
}


bool m44d::invert() {
	double determinant = det();
	if(determinant == 0) {
		return false;
	}

	assert(false);//TODO
	return true;
}

bool m44d::inverse(m44d &out) {
	double determinant = out.det();
	if(determinant == 0) {
		return false;
	}

	assert(false);//TODO
	return true;
}

// transpose it in place... (swap rows & columns)
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

// transpose but make a copy first
m44d m44d::transposition(const m44d &other) {
	m44d ret;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			ret.n[j][i] = other.n[i][j];

	return ret;
}

// elementary row operations for gaussian elimination

void m44d::multiply_row(m44d& rhs, size_t row, double n) {
	assert(row <= 4);

	for(size_t i = 0; i < 4; i++) {
		this->n[i][row] *= n;
		rhs[i][row] *= n;
	}
}

void m44d::swap_rows(m44d& rhs, size_t a, size_t b) {
	assert(a <= 4);
	assert(b <= 4);
	if(a == b) return;

	for(size_t i = 0; i < 4; i++) {
		std::swap<double>(n[i][a], n[i][b]);
		std::swap<double>(rhs[i][a], rhs[i][b]);
	}
}

void m44d::add_row(m44d& rhs, size_t from, size_t to, double n) {
	assert(from <= 4);
	assert(to <= 4);

	for(size_t i = 0; i < 4; i++) {
		this->n[i][to] += this->n[i][from] * n;
		rhs[i][to] += rhs[i][from] * n;
	}
}

// m44d operators (non scalar)

// equality check
bool m44d::operator==(const m44d &other) const {
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			if(other[i][j] != n[i][j])
				return false;
	return true;
}

bool m44d::operator!=(const m44d &other) const {
	return !(*this == other);
}


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

// multiplication

m44d m44d::operator*(const m44d &rhs) const {
	m44d ret = m44d::zero;

	/*  [xxxx] = [xxxx] * [xxxx]
	 *  [xxxx]   [xxxx]   [xxxx]
	 *  [xxxx]   [xxxx]   [xxxx]
	 *  [xxxx]   [xxxx]   [xxxx]
	 */

	for(int i = 0; i < 4; i++)     //row in dest
		for(int j = 0; j < 4; j++)   //col in dest
			for(int k = 0; k < 4; k++) //idx of val in current row/col
				ret.n[j][i] += n[k][i] * rhs.n[j][k];

	return ret;
}

m44d& m44d::operator*=(const m44d &other) {
	*this = *this * other;
	return *this;
}

// m44d * m41d
m41d m44d::operator*(const m41d& rhs) const {
	m41d out(m1d::zero);
	/*  [xxxx] * [x] = [x]
	 *  [xxxx]   [x]   [x]
	 *  [xxxx]   [x]   [x]
	 *  [xxxx]   [x]   [x]
	 */
	for(size_t i = 0; i < 4; i++)
		for(size_t k = 0; k < 4; k++)
			out[i] += n[i][k] * rhs[k];

	return out;
}

vector m44d::operator*(const vector& other) const {
	return static_cast<vector>(*this * static_cast<m41d>(other));
}
point m44d::operator*(const point& other) const {
	return static_cast<point>(*this * static_cast<m41d>(other));
}


// negation
m44d m44d::operator-() const {
	return -1 * *this;
}


// scalar operators

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


std::ostream& operator<<(std::ostream &os, const m44d &mat) {
	os << '\n';

	for(int i = 0; i < 4; i++)
		os << "["<<mat[i][0]<<","<<mat[i][1]<<","<<mat[i][2]<<","<<mat[i][3]<<"]"<< std::endl;

	return os;
}
