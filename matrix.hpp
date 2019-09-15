#pragma once

#include "lib/vector/vector.h"


class m44d {
	public:
		static const m44d zero;
		static const m44d unit;

		double n[4][4];

		double det() const;
		bool invert();
		static bool inverse(m44d &out);

		m44d& transpose();
		static m44d transposition(const m44d&);

		m44d operator+(const m44d&) const;
		m44d& operator+=(const m44d&);

		m44d operator-(const m44d&) const;
		m44d& operator-=(const m44d&);

		m44d operator*(const m44d&) const;
		m44d& operator*=(const m44d&);

		m44d operator-() const;// a = -my_m44d;

		m44d operator*(double) const;
		m44d operator/(double) const;
		friend m44d operator*(double, const m44d&);
		friend m44d operator/(double, const m44d&);

		m44d& operator*=(double);
		m44d& operator/=(double);
};

