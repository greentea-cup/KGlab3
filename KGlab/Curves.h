#ifndef CURVES_H
#define CURVES_H

#include "vec.h"

void draw_beizer2(vec3 const p[2], size_t samples);
void draw_bezier3(vec3 const p[3], size_t samples);
double N_f(double const U[], size_t U_len, unsigned int degree, size_t point_index, double t);
double N(double const U[], size_t U_len, unsigned int degree, size_t point_index, double t);
vec3 bspline(double const U[], vec3 const P[], size_t P_len, unsigned int degree, double t);
vec3 rbspline(double const U[], vec3 const P[], double const W[], size_t P_len, unsigned int degree, double t);
bool fill_knots(double *U, size_t U_len, size_t P_len, unsigned int degree);

#endif
