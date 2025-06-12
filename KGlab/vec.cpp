#include "vec.h"

vec3 vec3::operator+(vec3 const &o) const { return vec3 {x + o.x, y + o.y, z + o.z}; }
vec3 vec3::operator-(vec3 const &o) const { return vec3 {x - o.x, y - o.y, z - o.z}; }
vec3 vec3::operator*(double a) const { return vec3 {x * a, y * a, z * a}; }
vec3 vec3::operator/(double a) const { return vec3 {x / a, y / a, z / a}; }
vec3::operator double *(void) { return &x; }
vec3::operator double const *(void) const { return &x; }
double vec3::len2() { return x * x + y * y + z * z; }
double vec3::len() { return sqrt(len2()); }
vec3 vec3::norm() { return *this / len(); }
vec3 operator*(double a, vec3 const &b) { return b * a; }

vec3 midpoint(vec3 a, vec3 b) { return a + (b - a) / 2; }
vec3 cross(vec3 a, vec3 b) {
	/* https://en.wikipedia.org/wiki/Cross_product */
	return vec3 { a.y *b.z - a.z * b.y, a.z *b.x - a.x * b.z, a.x *b.y - a.y * b.x };
}
vec3 normal(vec3 a, vec3 b, vec3 c) { return cross(b - a, c - b).norm(); }

vec2 vec2::operator+(vec2 const &o) const { return vec2 {x + o.x, y + o.y}; }
vec2 vec2::operator-(vec2 const &o) const { return vec2 {x - o.x, y - o.y}; }
vec2 vec2::operator*(double a) const { return vec2 {x * a, y * a}; }
vec2 vec2::operator/(double a) const { return vec2 {x / a, y / a}; }
vec2::operator double *(void) { return &x; }
vec2::operator double const *(void) const { return &x; }
double vec2::len2() { return x * x + y * y; }
double vec2::len() { return sqrt(len2()); }
vec2 vec2::norm() { return *this / len(); }
