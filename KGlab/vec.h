#ifndef VEC_H
#define VEC_H

#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct vec3 {
	double x, y, z;
	vec3 operator+(vec3 const &o) const;
	vec3 operator-(vec3 const &o) const;
	vec3 operator*(double a) const;
	vec3 operator/(double a) const;
	operator double *(void);
	operator double const *(void) const;
	double len2();
	double len();
	vec3 norm();
};

struct vec2 {
	double x, y;
	vec2 operator+(vec2 const &o) const;
	vec2 operator-(vec2 const &o) const;
	vec2 operator*(double a) const;
	vec2 operator/(double a) const;
	operator double *(void);
	operator double const *(void) const;
	double len2();
	double len();
	vec2 norm();
};

vec3 operator*(double a, vec3 const &b);
vec3 midpoint(vec3 a, vec3 b);
vec3 cross(vec3 a, vec3 b);
vec3 normal(vec3 a, vec3 b, vec3 c);

#endif
