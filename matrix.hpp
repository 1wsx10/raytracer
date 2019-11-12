#pragma once

#include <iostream>
#include <utility>

#include "lib/vector/vector.h"

class m41d;
class m14d;
class vector;
class point;

#if 0 // TODO
template <size_t dim>
class msd {
	// square matrixd
	public:
		static const msd<dim> zero;
		static const msd<dim> unit;

		double n[dim][dim];
};
#endif

class m44d {
	public:
		static const m44d zero;
		static const m44d unit;

		double n[4][4];

		/** create a transformation matrix.
		 * where t is the transformation you want to encode
		 *
		 * @param i unit x vector that has been transformed by t
		 * @param j unit y vector that has been transformed by t
		 * @param k unit z vector that has been transformed by t
		 *
		 * @return the transformation matrix for t
		 */
		static m44d make_transformation(
				const v3d& i, const v3d& j, const v3d& k);

		/** create a transformation matrix.
		 * where t is the transformation you want to encode
		 *
		 * @param t transformation function
		 *
		 * @return the transformation matrix for t
		 */
		template<typename Func>
		static m44d make_transformation(Func t);

		/** create a transformation matrix for translation
		 * where tx is the translation you want to encode
		 *
		 * @param tx translation represented as a vector
		 *
		 * @return the transformation matrix for tx
		 */
		static m44d make_translation(const v3d& tx);

		double det() const;
		bool invert();
		static bool inverse(m44d &out);

		m44d& transpose();
		static m44d transposition(const m44d&);

		/* elementary row operations for gaussian elimination
		 */
		template<size_t row>
			void multiply_row(m44d& rhs, double n);

		template<size_t a, size_t b>
			void swap_rows(m44d& rhs);

		template<size_t from, size_t to>
			void add_row(m44d& rhs, double n);
		//==============================================


		bool operator==(const m44d&) const;
		bool operator!=(const m44d&) const;

		m44d operator+(const m44d&) const;
		m44d& operator+=(const m44d&);

		m44d operator-(const m44d&) const;
		m44d& operator-=(const m44d&);

		m44d operator*(const m44d&) const;
		m44d& operator*=(const m44d&);

		// m44d * m41d
		// the m14d version is in the m14d class
		m41d operator*(const m41d&) const;
		vector operator*(const vector&) const;
		point operator*(const point&) const;

		m44d operator-() const;// a = -my_m44d;

		m44d operator*(double) const;
		m44d operator/(double) const;
		friend m44d operator*(double, const m44d&);
		//friend m44d operator/(double, const m44d&); doesn't exist

		m44d& operator*=(double);
		m44d& operator/=(double);

		// display
		friend std::ostream& operator<<(std::ostream&, const m44d&);

		// implicit double[][] conversion
		using double_arr = double[4];
		constexpr operator double_arr*() { return n; };
		constexpr operator const double_arr*() const { return n; };
};
// copy assignment
static_assert(std::is_trivially_assignable<m44d, const m44d&>::value, "");
// copy ctor
static_assert(std::is_trivially_copy_constructible<m44d>::value, "");
// normal ctor
static_assert(std::is_trivially_copyable<m44d>::value, "");
// dtor
static_assert(std::is_trivially_destructible<m44d>::value, "");


// lambda function support for make_tx
template<typename Func>
m44d m44d::make_transformation(Func t) {
	return m44d::make_transformation(
	    t(v3d::X),
	    t(v3d::Y),
	    t(v3d::Z)
	    );
}




class m1d {
	public:
		static const m1d zero;

		double n[4];

		m1d(void) {};
		// from provided doubles
		constexpr m1d(const double& a,
		              const double& b,
		              const double& c,
		              const double& d) : n{a,b,c,d} {};
		// from v3d
		constexpr m1d(const v3d& from) : m1d(from, 1) {};
		constexpr m1d(const v3d& from, const double& w)
		              : m1d(from.x, from.y, from.z, w) {};

		// 'correct' homogeneous coordinates
		template<bool check>
		m1d& correct();
		template<bool check>
		m1d corrected() const;

		// maths with scalars
		m1d operator*(const double&) const;
		m1d& operator*=(const double&);
		m1d operator/(const double&) const;
		m1d& operator/=(const double&);

		// convert to v3d
		operator v3d&();
		operator const v3d&() const;

		// convert to m14d
		explicit operator m14d&();
		explicit operator const m14d&() const;
		// convert to m41d
		explicit operator m41d&();
		explicit operator const m41d&() const;

		// convert to double array for easy access
		constexpr operator const double*() const { return n; };
		constexpr operator double*() { return n; }
};


class m41d : public m1d {
	public:

		m41d(void) {};
		constexpr explicit m41d(m1d &from) : m1d(from) {};
		constexpr m41d(const v3d& from, const double& w): m1d(from, w) {}
		constexpr m41d(const double& a,
		               const double& b,
		               const double& c,
		               const double& d): m1d(a,b,c,d) {};
		m41d& operator=(const m1d&);

		m14d& transposed();
		const m14d& transposed() const;

		// arithmetic with self
		m41d operator+(m41d) const;
		m41d operator-(m41d) const;
		m41d& operator+=(const m41d&);
		m41d& operator-=(const m41d&);

		// m41d * m14d
		m44d operator*(const m14d&);
};

class vector : public m41d {
	public:

		constexpr vector(const v3d& from): m41d(from, 0) {};

		// vector + vector
		vector operator+(const vector&);

		// vector - vector
		vector operator-(const vector&);

		// vector + point
		point operator+(const point&);

		// vector - point
		// needs to be corrected afterward..?
		point operator-(const point&);

		m1d& correct() = delete;
};

class point : public m41d {
	public:
		constexpr point(const v3d& from): m41d(from, 1) {};

		// point + vector
		point operator+(const vector&);

		// point - vector
		point operator-(const vector*);

		// point + point
		// needs to be corrected afterward..?
		point operator+(const point&);

		// point - point
		vector operator-(const point&);
};

class m14d : public m1d {
	public:


		m14d(void) {};
		constexpr explicit m14d(m1d &from) : m1d(from) {};
		constexpr m14d(const v3d &from, const double& w) : m1d(from, w) {};
		constexpr m14d(const double& a, const double& b, const double& c, const double& d): m1d(a,b,c,d) {};
		m14d& operator=(const m1d&);

		m41d& transposed();
		const m41d& transposed() const;

		// arithmetic with self
		m14d operator+(m14d) const;
		m14d operator-(m14d) const;
		m14d& operator+=(const m14d&);
		m14d& operator-=(const m14d&);

		// m14d * m44d
		m14d operator*(const m44d& rhs) const;
		// m14d * m41d
		double operator*(const m41d& rhs) const;
};

