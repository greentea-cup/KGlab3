// vim: set ft=cpp fdm=indent :
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "lab3tr.h"
#include "vec.h"

void l3tr_1a() {
	// 1 a
	double xmin = -5, xmax = 5;
	size_t samples = 100;
	double scale = (xmax - xmin) / samples;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i <= samples; i++) {
		double x = 0;
		double y = xmin + i * scale;
		double z = pow(y, 4) - 3*pow(y,2);
		glVertex3d(x, y, z);
	}
	glEnd();
	glLineWidth(linewidth);
}

void l3tr_1b() {
	// 1 b
	double xmin = 0, xmax = 10;
	size_t samples = 500;
	double scale = (xmax - xmin) / samples;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i <= samples; i++) {
		double x = xmin + i * scale;
		double y = 0;
		double z = 2.0 * sin(x * M_PI);
		glVertex3d(x, y, z);
	}
	glEnd();
	glLineWidth(linewidth);
}

void l3tr_2() {
	// 2
	double xmin = 0, xmax = 10;
	size_t samples = 500;
	size_t samples2 = 31 - 1;
	double scale = (xmax - xmin) / samples;
	double scale2 = (xmax - xmin) / samples2;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i <= samples; i++) {
		double x = xmin + i * scale;
		double y = 0;
		double z = 2.0 * sin(x * M_PI);
		glVertex3d(x, y, z);
	}
	glEnd();
	GLfloat pointsize;
	glGetFloatv(GL_POINT_SIZE, &pointsize);
	glPointSize(10);
	glBegin(GL_POINTS);
	for (size_t i = 0; i <= samples2; i++) {
		double x = xmin + i * scale2;
		double y = 0;
		double z = 2.0 * sin(x * M_PI);
		glVertex3d(x, y, z);
	}
	glEnd();
	glPointSize(pointsize);
	glLineWidth(linewidth);
}

void l3tr_3a() {
	// 3 a
	double tmin = 0.0, tmax = 12.0 * M_PI;
	size_t samples = 1000 - 1;
	double scale = (tmax - tmin) / samples;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i <= samples; i++) {
		double t = tmin + i * scale;
		double sint = sin(t);
		double cost = cos(t);
		double k = exp(cost) - 2 * cos(4*t) - pow(sin(t / 12), 5);
		double x = sint * k;
		double y = cost * k;
		double z = 0.0;
		glVertex3d(x, y, z);
	}
	glEnd();
	glLineWidth(linewidth);
}

void l3tr_3b() {
	// 3 b
	double tmin = 0.0, tmax = 2.0 * M_PI;
	size_t samples = 100 - 1;
	double scale = (tmax - tmin) / samples;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i <= samples; i++) {
		double t = tmin + i * scale;
		double x = 16 * pow(sin(t), 3);
		double y = 13 * cos(t) - 5 * cos(2*t) - 2 *cos(3*t) - cos(4*t);
		double z = 0.0;
		glVertex3d(x, y, z);
	}
	glEnd();
	glLineWidth(linewidth);
}

void l3tr_4a() {
	// 4 a
	double xmin = -2, xmax = 2;
	double ymin = -2, ymax = 2;
	double xsamples = 20;
	double ysamples = 20;
	double xscale = (xmax - xmin) / xsamples;
	double yscale = (ymax - ymin) / ysamples;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glBegin(GL_LINES);
	for (size_t i = 0; i <= xsamples; i++) {
		double x = xmin + i * xscale;
		double zmin = -(0.5 * x + ymin - 1);
		double zmax = -(0.5 * x + ymax - 1);
		glVertex3d(x, ymin, zmin);
		glVertex3d(x, ymax, zmax);
	}
	for (size_t j = 0; j <= ysamples; j++) {
		double y = ymin + j * yscale;
		double zmin = -(0.5 * xmin + y - 1);
		double zmax = -(0.5 * xmax + y - 1);
		glVertex3d(xmin, y, zmin);
		glVertex3d(xmax, y, zmax);
	}
	glEnd();
	glLineWidth(linewidth);
}

void l3tr_4b() {
	// 4 b
	double xmin = -2, xmax = 2;
	double ymin = -2, ymax = 2;
	double xsamples = 20;
	double ysamples = 20;
	double xscale = (xmax - xmin) / xsamples;
	double yscale = (ymax - ymin) / ysamples;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glBegin(GL_LINES);
	for (size_t i = 0; i <= xsamples; i++) {
		double x = xmin + i * xscale;
	  	double x1 = x + xscale;
		for (size_t j = 0; j <= ysamples; j++) {
			double y = ymin + j * yscale;
	  		double y1 = y + yscale;
			double z = 0.5 * (x*x - y*y);
			double zx = 0.5 * (x1*x1 - y*y);
			double zy = 0.5 * (x*x - y1*y1);
			if (i != xsamples) {
				glVertex3d(x, y, z);
				glVertex3d(x1, y, zx);
			}
			if (j != ysamples) {
				glVertex3d(x, y, z);
				glVertex3d(x, y1, zy);
			}
		}
	}
	glEnd();
	glLineWidth(linewidth);
}

vec3 f5a(double u, double v) {
	return vec3{
		cos(u) * (cos(v) + 3),
		sin(u) * (cos(v) + 3),
		sin(v)
	};
}

void l3tr_5a() {
	// 5 a
	double umin = -M_PI, umax = +M_PI;
	double vmin = -M_PI, vmax = +M_PI;
	double usamples = 20;
	double vsamples = 20;
	double uscale = (umax - umin) / usamples;
	double vscale = (vmax - vmin) / vsamples;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glDisable(GL_LIGHTING);
	for (size_t i = 0; i < usamples; i++) {
		double u = umin + i * uscale;
	  	double u1 = u + uscale;
		for (size_t j = 0; j < vsamples; j++) {
			double v = vmin + j * vscale;
	  		double v1 = v + vscale;
			vec3 u0v0 = f5a(u, v);
			vec3 u1v0 = f5a(u1, v);
			vec3 u0v1 = f5a(u, v1);
			vec3 u1v1 = f5a(u1, v1);
			glColor3d(0.5, 0.5, 0.5);
			glBegin(GL_TRIANGLES);
			glVertex3dv(u0v0);
			glVertex3dv(u1v0);
			glVertex3dv(u1v1);
			glVertex3dv(u1v1);
			glVertex3dv(u0v1);
			glVertex3dv(u0v0);
			glEnd();
			glColor3d(0.0, 0.0, 1.0);
			glBegin(GL_LINE_STRIP);
			glVertex3dv(u0v0);
			glVertex3dv(u1v0);
			glVertex3dv(u1v1);
			glVertex3dv(u0v1);
			glVertex3dv(u0v0);
			glEnd();
		}
	}
	glLineWidth(linewidth);
}

vec3 f5b(double u, double v) {
	double k = 1.5 + 0.5 * sin(1.5 * u);
	return vec3{
		cos(u)*sin(v) + 3*cos(u)*k,
		sin(u)*sin(v) + 3*sin(u)*k,
		sin(v) + 2*cos(1.5*u)
	};
}

void l3tr_5b() {
	// 5 b
	double umin = -2.0 * M_PI, umax = +2.0 * M_PI;
	double vmin = -M_PI, vmax = +M_PI;
	double usamples = 100;
	double vsamples = 50;
	double uscale = (umax - umin) / usamples;
	double vscale = (vmax - vmin) / vsamples;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glDisable(GL_LIGHTING);
	for (size_t i = 0; i < usamples; i++) {
		double u = umin + i * uscale;
	  	double u1 = u + uscale;
		for (size_t j = 0; j < vsamples; j++) {
			double v = vmin + j * vscale;
	  		double v1 = v + vscale;
			vec3 u0v0 = f5b(u, v);
			vec3 u1v0 = f5b(u1, v);
			vec3 u0v1 = f5b(u, v1);
			vec3 u1v1 = f5b(u1, v1);
			glColor3d(0.5, 0.5, 0.5);
			glBegin(GL_TRIANGLES);
			glVertex3dv(u0v0);
			glVertex3dv(u1v0);
			glVertex3dv(u1v1);
			glVertex3dv(u1v1);
			glVertex3dv(u0v1);
			glVertex3dv(u0v0);
			glEnd();
			glColor3d(0.3, 0.3, 1.0);
			glBegin(GL_LINE_STRIP);
			glVertex3dv(u0v0);
			glVertex3dv(u1v0);
			glVertex3dv(u1v1);
			glVertex3dv(u0v1);
			glVertex3dv(u0v0);
			glEnd();
		}
	}
	glLineWidth(linewidth);
}

void l3tr_6_1() {
	// Криваая Безье квадратичная
	vec3 control_points[3] = {
		 1.00,  1.00,  0.00,
		 1.50,  2.00,  1.00,
		 2.50,  1.50,  0.00
	}, *p = control_points;
	vec3 A = 2[p];
	vec3 B = 2 * (1[p] - 2[p]);
	vec3 C = 0[p] - 2 * 1[p] + 2[p];
	size_t samples = 50;
	double scale = 1. / samples;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i <= samples; i++) {
		double t = scale * i;
		glVertex3dv(A + B*t + C*t*t);
	}
	glEnd();
	glLineWidth(linewidth);
	glBegin(GL_LINE_STRIP);
	glVertex3dv(0[p]);
	glVertex3dv(1[p]);
	glVertex3dv(2[p]);
	glEnd();
}

void l3tr_6_2() {
	// Герметичная кривая
	vec3 control_points[2] = {
		 0.34, -0.37,  1.00,
		 1.97, -1.22, -0.50,
	}, *p = control_points;
	vec3 tangent_vectors[2] = {
		-0.49, -0.70,  1.00,
		 1.01,  0.17,  0.50,
	}, *r = tangent_vectors;
	vec3 D = 2*0[p] + 0[r] - 2*1[p] +1[r];
	vec3 C = -3*0[p] + 3*1[p] - 2*0[r] - 1[r];
	vec3 B = 0[r];
	vec3 A = 0[p];
	size_t samples = 50;
	double scale = 1. / samples;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i <= samples; i++) {
		double t = scale * i;
		glVertex3dv(A + B*t + C*t*t + D*t*t*t);
	}
	glEnd();
	glLineWidth(linewidth);
	glBegin(GL_LINES);
	glVertex3dv(0[p]);
	glVertex3dv(0[p] + 0[r]);
	glVertex3dv(1[p]);
	glVertex3dv(1[p] + 1[r]);
	glEnd();
}

void l3tr_6_3() {
	// Криваая Безье кубическая
	vec3 control_points[4] = {
		 0.50,  0.50,  0.00,
		 0.86, -1.04,  2.00,
		 1.88, -0.96,  1.25,
		 1.60, -0.04,  0.30
	}, *p = control_points;
	vec3 A = 3[p];
	vec3 B = 3 * (2[p] - 3[p]);
	vec3 C = 3*1[p] - 6*2[p] + 3*3[p];
	vec3 D = 0[p] - 3*1[p] + 3*2[p] - 3[p];
	size_t samples = 50;
	double scale = 1. / samples;
	GLfloat linewidth;
	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i <= samples; i++) {
		double t = scale * i;
		glVertex3dv(A + B*t + C*t*t + D*t*t*t);
	}
	glEnd();
	glLineWidth(linewidth);
	glBegin(GL_LINE_STRIP);
	glVertex3dv(0[p]);
	glVertex3dv(1[p]);
	glVertex3dv(2[p]);
	glVertex3dv(3[p]);
	glEnd();
}

void l3tr_6() {
	l3tr_6_1();
	l3tr_6_2();
	l3tr_6_3();
}

void l3tr() {
	l3tr_6();
}
