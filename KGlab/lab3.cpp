// vim: set ft=cpp fdm=indent :
#include <cmath>
#include "lab3.h"
#include "lab2.h"

#define LEN(x) (sizeof((x))/sizeof(*(x)))
// %s/.*(\([^)]*\))\s*\(@{[^}]*}\)\n*/\1\r
static vec3 control_points[] = {
	4.38, 0.92, 0.0,
	0.75, 1.23, 0.0,
	2.50, 3.00, 1.0,
	1.50, 3.50, 1.0,
	2.00, 2.50, 0.0,
	2.28, 4.27, 1.0,
	2.96, 4.28, 1.0,
	4.75, 4.09, 0.0,
	4.40, 2.80, 0.0,
};
static size_t const P_len = LEN(control_points);

unsigned int const degree = 2;
static size_t const U_len = P_len + (size_t)degree + 1UL;
static double U[U_len];
static size_t const samples = 500;
static vec3 b_verts[samples+1];
static vec3 b_right[samples];
static vec3 b_up[samples];
static vec3 b_front[samples];
double scale = 1.0 / samples;
// Время, отведённое на путь туда и обратно по кривой
// в секундах
double const path_time = 20;
double const half_path_time = path_time * 0.5;
double const path_to_sample_scale = samples / half_path_time;

void prepare_lab3() {
	if (!fill_knots(U, U_len, P_len, degree)) return;
	b_verts[0] = bspline(U, control_points, P_len, degree, 0.0);
	for (size_t i = 1; i <= samples; i++) {
		double t = scale * i;
		b_verts[i] = bspline(U, control_points, P_len, degree, t);
		vec3 delta = b_verts[i] - b_verts[i-1];
		vec3 dflat = {delta.x, delta.y, 0};
		vec3 v0 = {0, 0, 1};
		vec3 right = cross(dflat, v0).norm();
		vec3 up = cross(right, delta).norm();
		b_front[i-1] = delta.norm();
		b_right[i-1] = right;
		b_up[i-1] = up;
	}
}

void l3task30() {
	vec3 control_verts[] = {
		0.5, 0.8, 0.0,
		0.48825, 0.5815, 0.0,
		0.456, 0.4425, 0.0,
		0.39, 0.16325, 0.0,
		0.2675, 0.1575, 0.0,
		0.13375, 0.155, 0.0,
		0.125, 0.35, 0.0,
		0.125, 0.55, 0.0,
		0.225, 0.675, 0.0,
		0.32, 0.785, 0.0,
		0.5, 0.8, 0.0,
		0.655, 0.811, 0.0,
		0.771, 0.694, 0.0,
		0.865, 0.59, 0.0,
		0.857, 0.43, 0.0,
		0.843, 0.274, 0.0,
		0.75, 0.2, 0.0,
		0.652, 0.137, 0.0,
		0.518, 0.128, 0.0,
	};
	size_t control_verts_len = sizeof(control_verts) / sizeof(*control_verts);
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glPushMatrix();
	glScaled(2.0, 2.0, 2.0);
	glTranslated(-0.5, -0.5, 0.0);
	for (size_t i = 0; i < control_verts_len - 2; i += 2) {
		draw_bezier3(control_verts + i, 20);
	}
	glPopMatrix();
	glLineWidth(linewidth);
}

void l3task40() {
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glPushMatrix();
	// glScaled(2.0, 2.0, 2.0);
	glTranslated(-2.5, -2.5, 0.0);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i <= samples; i++) glVertex3dv(b_verts[i]);
	glEnd();
	glLineWidth(linewidth);

	double mt = fmod(total_time, path_time);
	double tt = half_path_time  - fabs(mt - half_path_time);
	int dt = mt <= half_path_time ? 1 : -1;
	size_t pp = (size_t)floor(path_to_sample_scale * tt);
	vec3 p1 = b_verts[pp], p2;
	if (dt == 1 && pp == samples) p2 = b_verts[pp];
	else if (dt == -1 && pp == 0) p2 = b_verts[pp];
	else p2 = b_verts[pp + dt];
	vec3 prism_front = (p2 - p1).norm();
	
	glPushMatrix();
	glTranslated(p1.x, p1.y, p1.z);
	double mat0[16] = {
		b_front[pp].x, b_front[pp].y, b_front[pp].z, 0,
		b_up[pp].x, b_up[pp].y, b_up[pp].z, 0,
		b_right[pp].x, b_right[pp].y, b_right[pp].z, 0,
		0, 0, 0, 1
	};
	glMultMatrixd(mat0);
	if (dt == -1) glRotated(180, b_up[pp].x, b_up[pp].y, b_up[pp].z);

	glPushMatrix();
	glScaled(0.025, 0.025, 0.025);
	l2task2();
	glPopMatrix();
	glPopMatrix();
	// Нарисовать вектора, смотрящие вправо и вверх относительно отрезков кривой
	// glBegin(GL_LINES);
	// for (size_t i = 0; i < samples; i++) {
	// 	glVertex3dv(b_verts[i]);
	// 	glVertex3dv(b_verts[i]+b_right[i]);
	// 	glVertex3dv(b_verts[i]);
	// 	glVertex3dv(b_verts[i]+b_up[i]);
	// }
	// glEnd();
	glPopMatrix();
}

