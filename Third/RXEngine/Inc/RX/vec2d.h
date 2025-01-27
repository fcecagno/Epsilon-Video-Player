/*
=================================================
vec2d.h
-------------------------------------------------
Author:			Ros�lia Galiazzi Schneider
Last Modified:	06/03/2009
=================================================
*/

#ifndef __VEC2D_H
#define __VEC2D_H

#include <iostream>
#include "Mathematics.h"

namespace RX
{

class vec2d
{
public:
	// Constructors
	vec2d(double x1, double y1);
	vec2d();

	// Norm & Squared norm
	const inline double norm() const { return sqrt(x*x + y*y); }
	const inline double sqrNorm() const { return x*x + y*y; }

	// Normalize vector
	inline void normalize() {
		double n = norm();
		if(n) {
			x /= n; 
			y /= n;
		}
	}

	// Addition/Subtraction
	vec2d operator+(const vec2d &v) const;
	vec2d operator+=(const vec2d &v);
	vec2d operator-(const vec2d &v) const;
	vec2d operator-=(const vec2d &v);

	// Multiplication/Division by scalar
	vec2d operator*=(const double &f);
	vec2d operator/=(const double &f);
	vec2d operator/(const double &f);


	double *doubles() {
		f[0] = x; f[1] = y;
		return f;
	}

	double x, y;

private:
	double f[2];

};

vec2d operator*(const double &f, const vec2d &v);
vec2d operator*(const vec2d &v, const double &f);

std::ostream &operator<<(std::ostream &out, vec2d v);
std::istream &operator>>(std::istream &in, vec2d &v);

};

#endif // __VEC2D_H