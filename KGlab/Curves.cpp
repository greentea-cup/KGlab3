#include <Windows.h>
#include <malloc.h>
#define alloca _alloca
#include <GL\GL.h>
#include <GL\GLU.h>
#include <iostream>
#include "Curves.h"

void draw_beizer2(vec3 const p[2], size_t samples) {
	if (!p) return;
	vec3 A = p[1];
	vec3 B = p[1] - p[0];
	double scale = 1. / (samples - 1);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < samples; i++) {
		double t = scale * i;
		glVertex3dv(A + B * t);
	}
	glEnd();
}

void draw_bezier3(vec3 const p[3], size_t samples) {
	if (!p) return;
	vec3 A = p[2];
	vec3 B = 2 * (p[1] - p[2]);
	vec3 C = p[0] - 2 * p[1] + p[2];
	double scale = 1. / (samples);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i <= samples; i++) {
		double t = scale * i;
		glVertex3dv(A + B * t + C * t * t);
	}
	glEnd();
}

double N_f(double const U[], size_t U_len, unsigned int degree, size_t point_index, double t) {
	size_t i = point_index;
	double a = t - U[i];
	double b = U[i+degree] - U[i];
	if (b == 0.0) return 0.0;
	return a / b;
}

double N(double const U[], size_t U_len, unsigned int degree, size_t point_index, double t) {
	size_t i = point_index;
	if (i == U_len-2 && t == U[U_len-1]) return 1.0;
	else if (degree == 0) return (U[i] <= t && t < U[i+1]) ? 1.0 : 0.0;
	double a = N_f(U, U_len, degree, i, t);
	double b = 1-N_f(U, U_len, degree, i+1, t);
	double x = a == 0.0 ? 0.0 : N(U, U_len, degree-1, i, t);
	double y = b == 0.0 ? 0.0 : N(U, U_len, degree-1, i+1, t);
	return a*x + b*y;
}

vec3 bspline(double const U[], vec3 const P[], size_t P_len, unsigned int degree, double t) {
	vec3 res = {};
	for (size_t i = 0; i < P_len; i++) {
		double N_i = N(U, degree + P_len + 1, degree, i, t);
		res = res + N_i * P[i];
	}
	return res;
}

vec3 rbspline(double const U[], vec3 const P[], double const W[], size_t P_len, unsigned int degree, double t) {
	vec3 res = {};
	double ww = 0;
	for (size_t i = 0; i < P_len; i++) {
		double N_i = N(U, degree + P_len + 1, degree, i, t);
		double w_i = N_i * W[i];
		res = res + w_i * P[i];
		ww += w_i;
	}
	return res / ww;
}

static double u(size_t total_points, unsigned int degree, size_t knot_index) {
	size_t i = knot_index;
	if (i <= degree)
		return 0.0;
	if (i <= total_points)
		return (i - degree) / (double)(total_points - degree);
	return 1;
}

bool fill_knots(double *U, size_t U_len, size_t P_len, unsigned int degree) {
	if (U_len < P_len + (size_t)degree + 1UL) return false;
	for (size_t j = 0; j < U_len; j++) U[j] = u(P_len, degree, j);
	return true;
}

