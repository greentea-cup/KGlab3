/* vim: set fdm=indent : */

#define HEIGHT 3.0
#define NEXTRA1 512
#define NEXTRA2 512
#define TEXTURE_1 "texture_1.png"
#define TEXTURE_2 "texture_2.png"
#define TEXTURE_3 "texture_3.png"
#define	UV_30_ZERO {-7., -8., 0.}
double const UV_30_SCALE = 1. / 15.;
#define UV_50_ZERO {-7., -8., 0.}
double const UV_50_SCALE = 1. / 18.;
#define UV_50_START_BOTTOM 36
#define UV_50_END_BOTTOM 54
#define UV_50_START_TOP 54
#define UV_50_END_TOP 72
size_t const UV_50_LEN_BOTTOM = UV_50_END_BOTTOM - UV_50_START_BOTTOM;
size_t const UV_50_LEN_TOP = UV_50_END_TOP - UV_50_START_TOP;
double const SIDE_UV_SCALE = 1. / HEIGHT;

#define LEN(x) (sizeof((x))/sizeof(*(x)))

#include "Render.h"
#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "GUItextRectangle.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <random>
#include <algorithm>

#ifdef _DEBUG
#include <Debugapi.h>
struct debug_print {
	template<class C>
	debug_print &operator<<(const C& a) {
		OutputDebugStringA((std::stringstream() << a).str().c_str());
		return *this;
	}
} debout;
#else
struct debug_print {
	template<class C>
	debug_print &operator<<(const C& a) {
		return *this;
	}
} debout;
#endif
//https://github.com/nothings/stb
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//внутренняя логика "движка"
#include "MyOGL.h"
extern OpenGL gl;
#include "Light.h"
Light light;
#include "Camera.h"
Camera camera;

struct vec3 {
	double x, y, z;
	vec3 operator+(vec3 const &o) const { return vec3 {x + o.x, y + o.y, z + o.z}; }
	vec3 operator-(vec3 const &o) const { return vec3 {x - o.x, y - o.y, z - o.z}; }
	vec3 operator*(double a) const { return vec3 {x * a, y * a, z * a}; }
	vec3 operator/(double a) const { return vec3 {x / a, y / a, z / a}; }
	operator double *(void) { return &x; }
	operator double const *(void) const { return &x; }
	double len2() { return x * x + y * y + z * z; }
	double len() { return sqrt(len2()); }
	vec3 norm() { return *this / len(); }
};
struct vec2 {
	double x, y;
	vec2 operator+(vec2 const &o) const { return vec2 {x + o.x, y + o.y}; }
	vec2 operator-(vec2 const &o) const { return vec2 {x - o.x, y - o.y}; }
	vec2 operator*(double a) const { return vec2 {x * a, y * a}; }
	vec2 operator/(double a) const { return vec2 {x / a, y / a}; }
	operator double *(void) { return &x; }
	operator double const *(void) const { return &x; }
	double len2() { return x * x + y * y; }
	double len() { return sqrt(len2()); }
	vec2 norm() { return *this / len(); }
};
enum winding_order {
	WND_CCW = 0, /* Против часовой */
	WND_CW = 1, /* По часовой */
};

vec3 midpoint(vec3 a, vec3 b) {
	return a + (b - a) / 2;
}
vec3 cross(vec3 a, vec3 b) {
	/* https://en.wikipedia.org/wiki/Cross_product */
	return vec3 { a.y *b.z - a.z * b.y, a.z *b.x - a.x * b.z, a.x *b.y - a.y * b.x };
}
vec3 normal(vec3 a, vec3 b, vec3 c) {
	return cross(b - a, c - b).norm();
}
vec3 circle_center_xy(vec3 p1, vec3 p2, vec3 p3) {
	// https://www.desmos.com/calculator/6fgvz0xpmb
	double a = p1.x, b = p1.y, c = p2.x, d = p2.y, f = p3.x, g = p3.y;
	double m1 = (b - d) / (a - c),
		   m2 = (b - g) / (a - f),
		   m3 = (d - g) / (c - f),
		   m4 = (c - a) / (b - d),
		   m5 = (f - a) / (b - g),
		   m6 = (f - c) / (d - g);
	double c1 = -a * m1 + b,
		   c2 = -a * m2 + b,
		   c3 = -c * m3 + d,
		   c4 = -m4 * (a + c) * 0.5 + (b + d) * 0.5,
		   c5 = -m5 * (a + f) * 0.5 + (b + g) * 0.5,
		   c6 = -m6 * (c + f) * 0.5 + (d + g) * 0.5;
	double cx = (c6 - c5) / (m5 - m6),
		   cy = (m6 * c5 - m5 * c6) / (m6 - m5);
	return vec3(cx, cy, p1.z);
}
static void fill_arc_points_xy(vec3 *data, size_t npoints, winding_order cw, vec3 a, vec3 b,
	double distance_xy, vec3 center) {
	double ax = a.x - center.x, ay = a.y - center.y, bx = b.x - center.x, by = b.y - center.y;
	double radius2 = ax * ax + ay * ay;
	double radius = sqrt(radius2);
	double phase = atan2(ay, ax);
	double full_angle = acos((ax * bx + ay * by) / radius2);
	double step = full_angle / (npoints - 1);
	for (size_t i = 0; i < npoints; i++) {
		double angle = (cw == WND_CCW ? npoints - 1 - i : i) * step + phase;
		data[i].x = center.x + radius * cos(angle);
		data[i].y = center.y + radius * sin(angle);
		data[i].z = a.z;
	}
}
static void calc_normals(vec3 const *verts, size_t npoints, vec3 *normals) {
	for (size_t i = 0; i < npoints; i += 3)
		normals[i / 3] = normal(verts[i + 0], verts[i + 1], verts[i + 2]);
}
static void calc_uv(vec3 const *verts, size_t npoints, vec2 *uv, vec3 zero, double scale) {
	vec3 z = zero * scale;
	for (size_t i = 0; i < npoints; i++) {
		vec3 x = verts[i] * scale - z;
		uv[i] = vec2 {x.x, 1. - x.y};
	}
}
static void calc_uv_xrev(vec3 const *verts, size_t npoints, vec2 *uv, vec3 zero, double scale) {
	vec3 z = zero * scale;
	for (size_t i = 0; i < npoints; i++) {
		vec3 x = verts[i] * scale - z;
		uv[i] = vec2 {1. - x.x, 1. - x.y};
	}
}
static void reverse_uv_x(vec2 *uv, size_t npoints) {
	for (size_t i = 0; i < npoints; i++)
		uv[i].x = 1 - uv[i].x;
}
static double mind(double a, double b, double c) {
	double m1 = (a < b) ? a : b;
	return (m1 < c) ? m1 : c;
}
static double maxd(double a, double b, double c) {
	double m1 = (a > b) ? a : b;
	return (m1 > c) ? m1 : c;
}


// *INDENT-OFF*
static vec3 /* основные цвета */
	c1 = { 1. / 14., 1. / 14., 1. / 14. },
	c2 = { 2. / 14., 2. / 14., 2. / 14. },
	c3 = { 3. / 14., 3. / 14., 3. / 14. },
	c4 = { 4. / 14., 4. / 14., 4. / 14. },
	c5 = { 5. / 14., 5. / 14., 5. / 14. },
	c6 = { 6. / 14., 6. / 14., 6. / 14. },
	c7 = { 7. / 14., 7. / 14., 7. / 14. },
	c8 = { 8. / 14., 8. / 14., 8. / 14. },
	c9 = { 9. / 14., 9. / 14., 9. / 14. },
	c10 = { 10. / 14., 10. / 14., 10. / 14. },
	c11 = { 11. / 14., 11. / 14., 11. / 14. },
	c12 = { 12. / 14., 12. / 14., 12. / 14. },
	c13 = { 13. / 14., 13. / 14., 13. / 14. },
	c14 = { 14. / 14., 14. / 14., 14. / 14. };

static vec3 /* базовые точки */
	A = { 2., -1., 0. }, B = { -1., -6., 0. },
	C = { -7., -3., 0. }, D = { -5., 4., 0. },
	E = { 1., 2., 0. }, F = { 4., 6., 0. },
	G = { 8., 5., 0. }, H = { 7., -2., 0. },
	A1 = { 2., -1., HEIGHT }, B1 = { -1., -6., HEIGHT },
	C1 = { -7., -3., HEIGHT }, D1 = { -5., 4., HEIGHT },
	E1 = { 1., 2., HEIGHT }, F1 = { 4., 6., HEIGHT },
	G1 = { 8., 5., HEIGHT }, H1 = { 7., -2., HEIGHT };

static vec3 /* дополнительные точки */
	/* M - точка, через которую проходит вырез */
	M = {-5., -2., 0.}, M1 = {-5., -2., HEIGHT},
	/* X - центр выпуклости */
	X = midpoint(H, G), X1 = midpoint(H1, G1),
	/* Y - центр вогнутости */
	Y = circle_center_xy(C, D, M), Y1 = circle_center_xy(C1, D1, M1),
	/* K = mid(B, C) */
	K = midpoint(B, C), K1 = midpoint(B1, C1),
	/* L = mid(D, E) */
	L = midpoint(D, E), L1 = midpoint(D1, E1);
// *INDENT-ON*
static vec3 const prism30verts[] = {
	/* Вершины призмы (30) */
	/* низ */
	A, B, C, A, C, D, // 0
	A, D, E, A, E, H, // 6
	E, F, G, E, G, H, // 12
	/* стороны */
	A, H, H1, A, H1, A1, // 18
	H, G, G1, H, G1, H1, // 24
	G, F, F1, G, F1, G1, // 30
	F, E, E1, F, E1, F1, // 36
	E, D, D1, E, D1, E1, // 42
	D, C, C1, D, C1, D1, // 48
	C, B, B1, C, B1, C1, // 54
	B, A, A1, B, A1, B1, // 60
	/* верх */
	H1, G1, F1, H1, F1, E1, // 66
	H1, E1, D1, H1, D1, A1,
	D1, C1, B1, D1, B1, A1,
};
static vec3 const prism30colors[] = {
	/* Цвета призмы (30) */
	/* низ */
	c1, c1,
	c2, c2,
	c3, c3,
	/* стороны */
	c4, c4,
	c5, c5,
	c6, c6,
	c7, c7,
	c8, c8,
	c9, c9,
	c10, c10,
	c11, c11,
	/* верх */
	c12, c12,
	c13, c13,
	c14, c14,
};
static vec3 const prism50data[] = {
	/* Цвета и вершины призмы (50) */
	/* стороны */
	c4, A, H, H1, c4, A, H1, A1,
	/* c5, H, G, G1, c5, H, G1, H1, */
	c6, G, F, F1, c6, G, F1, G1,
	c7, F, E, E1, c7, F, E1, F1,
	c8, E, D, D1, c8, E, D1, E1,
	/* c9, D, C, C1, c9, D, C1, D1, */
	c10, C, B, B1, c10, C, B1, C1,
	c11, B, A, A1, c11, B, A1, B1,
	/* низ */ // 12 * 3 = 36 - uv start bottom
	/* c1, A, B, C, c1, A, C, D, */
	c1, A, B, K, c1, A, K, L,
	/* c2, A, D, E, */
	c2, A, L, E, c2, A, E, H,
	c3, E, F, G, c3, E, G, H, // 6 * 3 = 18 - uv lenght bottom
	/* верх */ // 18 * 3 = 54 - uv start top
	c12, H1, G1, F1, c12, H1, F1, E1,
	/* c13, H1, E1, D1, c13, H1, D1, A1, */
	c13, A1, E1, L1, c13, A1, H1, E1,
	/* c14, D1, C1, B1, c14, D1, B1, A1, */
	c14, A1, K1, B1, c14, A1, L1, K1, c14, A1, E1, L1,
	// 6 * 3 = 18 - uv length top
};
static vec3 extra1[2 * NEXTRA1], extra2[2 * NEXTRA2];
// нормаль 1 на треугольник
static vec3 prism30normals[sizeof(prism30verts) / (3 * sizeof(*prism30verts))];
static vec2 prism30TopUv[18]; // верхняя крышка занимает 18 вершин
size_t const prism50len = 3 * LEN(prism50data) / 4 + 12 * (NEXTRA1 + NEXTRA2) - 27 +
	(NEXTRA2 % 2 == 0 ? 12 : 6);
static vec3 prism50verts[prism50len];
static vec3 prism50normals[LEN(prism50verts) / 3];
static vec2 prism50uv[LEN(prism50verts)];
static vec2 prism50TopUv[UV_50_LEN_TOP];
static vec2 prism50BottomUv[UV_50_LEN_BOTTOM];

bool texturing = true;
bool lighting = true;
bool alpha = true;
bool smoothShade = true;
bool lightFollowsCamera = true;
bool insideCulling = false;
bool showNormals = false;
bool thickNormals = false;
bool togglePrism = true;

//переключение режимов освещения, текстурирования, альфаналожения
void switchModes(OpenGL *sender, KeyEventArg arg) {
	//конвертируем код клавиши в букву
	auto key = LOWORD(MapVirtualKeyA(arg.key, MAPVK_VK_TO_CHAR));

	switch (key) {
	case 'L':
		lighting = !lighting;
		break;
	case 'T':
		texturing = !texturing;
		break;
	case 'A':
		alpha = !alpha;
		break;
	case 'S':
		smoothShade = !smoothShade;
		break;
	case 'U':
		lightFollowsCamera = !lightFollowsCamera;
		break;
	case 'C':
		insideCulling = !insideCulling;
		break;
	case 'N':
		showNormals = !showNormals;
		break;
	case ';':
		thickNormals = !thickNormals;
		break;
	case '\\':
		togglePrism = !togglePrism;
		break;
	}
}

//Текстовый прямоугольничек в верхнем правом углу.
//OGL не предоставляет возможности для хранения текста
//внутри этого класса создается картинка с текстом (через виндовый GDI),
//в виде текстуры накладывается на прямоугольник и рисуется на экране.
//Это самый простой способ что то написать на экране
//но ооооочень не оптимальный
GuiTextRectangle text;

static GLuint prepareTexture(char const *path) {
	GLuint id;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	{
		/* Загрузка текстуры */
		int x, y, n;
		unsigned char *data = stbi_load(path, &x, &y, &n, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return id;
}

static void reloadTexture(GLuint id, char const *path) {
	/* Загрузка текстуры */
	int x, y, n;
	unsigned char *data = stbi_load(path, &x, &y, &n, 4);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
}

static void gen_side_uv(vec3 const *verts, size_t len, vec2 *uv) {
	double offset = 0;
	for (size_t i = 0; i < len; i += 6) {
		double xx = verts[i+0].x - verts[i+1].x;
		double yy = verts[i+0].y - verts[i+1].y;
		double xy_dist = sqrt(xx * xx + yy * yy) * SIDE_UV_SCALE;
		// A B C, A C D :
		// D C
		// A B
		uv[i+0] = {offset + xy_dist, 0.}; // A
		uv[i+1] = {offset, 0.}; // B
		uv[i+2] = {offset, 1.}; // C
		uv[i+3] = {offset + xy_dist, 0.}; // A
		uv[i+4] = {offset, 1.}; // C
		uv[i+5] = {offset + xy_dist, 1.}; // D
		offset += xy_dist;
	}
}

static size_t prism50BottomStart, prism50BottomEnd, prism50TopStart, prism50TopEnd;

static void fill_prism50(void) {
	size_t j = 0, j0;
	// Статичная часть призмы
	for (size_t i = 0; i < LEN(prism50data); i += 4) {
		prism50verts[j++] = prism50data[i + 1];
		prism50verts[j++] = prism50data[i + 2];
		prism50verts[j++] = prism50data[i + 3];
	}
	gen_side_uv(prism50verts, UV_50_START_BOTTOM, prism50uv);
	calc_uv_xrev(prism50verts + UV_50_START_BOTTOM, UV_50_LEN_BOTTOM, prism50uv + UV_50_START_BOTTOM,
		UV_50_ZERO, UV_50_SCALE);
	calc_uv(prism50verts + UV_50_START_TOP, UV_50_LEN_TOP, prism50uv + UV_50_START_TOP, UV_50_ZERO,
		UV_50_SCALE);
	{ /* Низ */
		prism50BottomStart = j0 = j;
		// Низ выпуклости
		for (size_t i = 1; i < NEXTRA1; i++) {
			prism50verts[j++] = X;
			prism50verts[j++] = extra1[i - 1];
			prism50verts[j++] = extra1[i];
		}

		// Низ вогнутости (1 - L)
		for (size_t i = 1; i < (NEXTRA2 + 1) / 2; i++) {
			prism50verts[j++] = L;
			prism50verts[j++] = extra2[i];
			prism50verts[j++] = extra2[i - 1];
		}

		// Низ вогнутости (2 - середина)
		{
			prism50verts[j++] = L;
			prism50verts[j++] = K;
			prism50verts[j++] = extra2[NEXTRA2 / 2];
			if (NEXTRA2 % 2 == 0) {
				prism50verts[j++] = L;
				prism50verts[j++] = extra2[NEXTRA2 / 2];
				prism50verts[j++] = extra2[NEXTRA2 / 2 - 1];
			}
		}

		// Низ вогнутости (3 - K)
		for (size_t i = NEXTRA2 / 2 + 1; i < NEXTRA2; i++) {
			prism50verts[j++] = K;
			prism50verts[j++] = extra2[i];
			prism50verts[j++] = extra2[i - 1];
		}
		calc_uv_xrev(prism50verts + j0, j - j0, prism50uv + j0, UV_50_ZERO, UV_50_SCALE);
		prism50BottomEnd = j;
	}

	{ /* Верх */
		prism50TopStart = j0 = j;
		// Верх выпуклости
		for (size_t i = 1; i < NEXTRA1; i++) {
			prism50verts[j++] = X1;
			prism50verts[j++] = extra1[2 * NEXTRA1 - i - 1];
			prism50verts[j++] = extra1[2 * NEXTRA1 - i];
		}

		// Верх вогнутости (1 - L)
		for (size_t i = 1; i < (NEXTRA2 + 1) / 2; i++) {
			prism50verts[j++] = L1;
			prism50verts[j++] = extra2[2 * NEXTRA2 - i];
			prism50verts[j++] = extra2[2 * NEXTRA2 - i - 1];
		}

		// Верх вогнутости (2 - середина)
		{
			prism50verts[j++] = K1;
			prism50verts[j++] = L1;
			prism50verts[j++] = extra2[NEXTRA2 + NEXTRA2 / 2];
			if (NEXTRA2 % 2 == 0) {
				prism50verts[j++] = K1;
				prism50verts[j++] = extra2[NEXTRA2 + NEXTRA2 / 2];
				prism50verts[j++] = extra2[NEXTRA2 + NEXTRA2 / 2 - 1];
			}
		}

		// Верх вогнутости (3 - K)
		for (size_t i = NEXTRA2 / 2 + 1; i < NEXTRA2; i++) {
			prism50verts[j++] = K1;
			prism50verts[j++] = extra2[2 * NEXTRA2 - i];
			prism50verts[j++] = extra2[2 * NEXTRA2 - i - 1];
		}
		calc_uv(prism50verts + j0, j - j0, prism50uv + j0, UV_50_ZERO, UV_50_SCALE);
		prism50TopEnd = j;
	}

	j0 = j;
	// стороны
	for (size_t i = 1; i < NEXTRA1; i++) {
		prism50verts[j++] = extra1[i];
		prism50verts[j++] = extra1[i - 1];
		prism50verts[j++] = extra1[2 * NEXTRA1 - i];
		prism50verts[j++] = extra1[i];
		prism50verts[j++] = extra1[2 * NEXTRA1 - i];
		prism50verts[j++] = extra1[2 * NEXTRA1 - i - 1];
	}
	gen_side_uv(prism50verts+j0, j-j0, prism50uv+j0);
	j0 = j;
	// Внешняя обшивка вогнутости (4)
	for (size_t i = 1; i < NEXTRA2; i++) {
		prism50verts[j++] = extra2[i - 1];
		prism50verts[j++] = extra2[i];
		prism50verts[j++] = extra2[2 * NEXTRA2 - i];
		prism50verts[j++] = extra2[i];
		prism50verts[j++] = extra2[2 * NEXTRA2 - i - 1];
		prism50verts[j++] = extra2[2 * NEXTRA2 - i];
	}
	gen_side_uv(prism50verts+j0, j-j0, prism50uv+j0);
}

GLuint texId1, texId2, texId3;
//выполняется один раз перед первым рендером
void initRender() {
	fill_arc_points_xy(extra1, NEXTRA1, WND_CCW, H, G, 0., X);
	fill_arc_points_xy(extra1 + NEXTRA1, NEXTRA1, WND_CW, H1, G1, 0., X1);
	fill_arc_points_xy(extra2, NEXTRA2, WND_CCW, C, D, 0., Y);
	fill_arc_points_xy(extra2 + NEXTRA2, NEXTRA2, WND_CW, C1, D1, 0., Y1);
	fill_prism50();
	calc_normals(prism30verts, LEN(prism30verts), prism30normals);
	calc_normals(prism50verts, LEN(prism50verts), prism50normals);
	// +66 - смещение до верхней грани
	// 18 - длина блока верхней грани
	calc_uv(prism30verts + 66, 18, prism30TopUv, UV_30_ZERO, UV_30_SCALE);
	calc_uv(prism50verts + UV_50_START_TOP, UV_50_LEN_TOP, prism50TopUv, UV_50_ZERO, UV_50_SCALE);
	calc_uv(prism50verts + UV_50_START_BOTTOM, UV_50_LEN_BOTTOM, prism50BottomUv, UV_50_ZERO,
		UV_50_SCALE);
	reverse_uv_x(prism50BottomUv, UV_50_LEN_BOTTOM);

	// НАСТРОЙКА ТЕКСТУР
	texId1 = prepareTexture(TEXTURE_1);
	texId2 = prepareTexture(TEXTURE_2);
	texId3 = prepareTexture(TEXTURE_3);
	// ---

	//================НАСТРОЙКА КАМЕРЫ======================
	camera.caclulateCameraPos();

	//привязываем камеру к событиям "движка"
	gl.WheelEvent.reaction(&camera, &Camera::Zoom);
	gl.MouseMovieEvent.reaction(&camera, &Camera::MouseMovie);
	gl.MouseLeaveEvent.reaction(&camera, &Camera::MouseLeave);
	gl.MouseLdownEvent.reaction(&camera, &Camera::MouseStartDrag);
	gl.MouseLupEvent.reaction(&camera, &Camera::MouseStopDrag);
	//==============НАСТРОЙКА СВЕТА===========================
	//привязываем свет к событиям "движка"
	gl.MouseMovieEvent.reaction(&light, &Light::MoveLight);
	gl.KeyDownEvent.reaction(&light, &Light::StartDrug);
	gl.KeyUpEvent.reaction(&light, &Light::StopDrug);
	//========================================================
	//====================Прочее==============================
	gl.KeyDownEvent.reaction(switchModes);
	text.setSize(512, 250);
	//========================================================

	camera.setPosition(2, 1.5, 1.5);
}

static void draw_normals(vec3 const *verts, vec3 const *normals, size_t npoints) {
	if (!showNormals) return;
	GLfloat oldWidth;
	if (thickNormals) {
		glGetFloatv(GL_LINE_WIDTH, &oldWidth);
		glLineWidth(4.f);
	}
	glBegin(GL_LINES);
	for (size_t i = 0; i < npoints; i += 3) {
		vec3 a = midpoint(midpoint(verts[i + 0], verts[i + 1]), verts[i + 2]);
		glVertex3dv(a);
		glVertex3dv(a + normals[i / 3]);
	}
	glEnd();
	if (thickNormals) glLineWidth(oldWidth);
}

static void task1() {
	float const amb[] = {1.0f, 1.0f, 1.0f, 0.5f};
	float const dif[] = {1.0f, 1.0f, 1.0f, 0.5f};
	draw_normals(prism30verts, prism30normals, sizeof(prism30verts) / sizeof(*prism30verts));
	if (lighting) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);
	if (insideCulling) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i < sizeof(prism30verts) / sizeof(vec3) - 18; i++) {
		glNormal3dv(prism30normals[i / 3]);
		glColor3dv(prism30colors[i / 3]);
		glVertex3dv(prism30verts[i]);
	}
	glEnd();
	if (texturing) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texId1);
	}
	if (alpha) glEnable(GL_BLEND);
	if (alpha && lighting) {
		glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	}
	glBegin(GL_TRIANGLES);
	for (size_t i = 0, j = 66; i < 18; i++, j++) {
		glNormal3dv(prism30normals[j / 3]);
		if (alpha) {
			vec3 c = prism30colors[j / 3];
			glColor4d(c.x, c.y, c.z, 0.5);
		}
		if (texturing) glTexCoord2dv(prism30TopUv[i]);
		glVertex3dv(prism30verts[j]);
	}
	glEnd();
}

static void task2_draw_bottom() {
	glBindTexture(GL_TEXTURE_2D, texId1);
	glBegin(GL_TRIANGLES);
	glColor4d(0.75, 0.75, 0.75, 0.75);
	for (size_t i = UV_50_START_BOTTOM; i < UV_50_END_BOTTOM; i++) {
		glNormal3dv(prism50normals[i / 3]);
		glTexCoord2dv(prism50uv[i]);
		glVertex3dv(prism50verts[i]);
	}
	for (size_t i = prism50BottomStart; i < prism50BottomEnd; i++) {
		glNormal3dv(prism50normals[i / 3]);
		glTexCoord2dv(prism50uv[i]);
		glVertex3dv(prism50verts[i]);
	}
	glEnd();
}

static void task2_draw_top() {
	glBindTexture(GL_TEXTURE_2D, texId2);
	glBegin(GL_TRIANGLES);
	for (size_t i = UV_50_START_TOP; i < UV_50_END_TOP; i++) {
		glNormal3dv(prism50normals[i / 3]);
		glTexCoord2dv(prism50uv[i]);
		glVertex3dv(prism50verts[i]);
	}
	for (size_t i = prism50TopStart; i < prism50TopEnd; i++) {
		glNormal3dv(prism50normals[i / 3]);
		glTexCoord2dv(prism50uv[i]);
		glVertex3dv(prism50verts[i]);
	}
	glEnd();
}

static void task2() {
	float const amb[] = {1.0f, 1.0f, 1.0f, 0.5f};
	float const dif[] = {1.0f, 1.0f, 1.0f, 0.5f};
	draw_normals(prism50verts, prism50normals, LEN(prism50verts));
	if (lighting) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);
	if (insideCulling) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);
	if (texturing) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texId3);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}
	if (alpha) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
	glBegin(GL_TRIANGLES);
	glColor3d(0.5, 0.5, 0.5);
	for (size_t i = 0; i < UV_50_START_BOTTOM; i++) {
		glNormal3dv(prism50normals[i / 3]);
		glTexCoord2dv(prism50uv[i]);
		glVertex3dv(prism50verts[i]);
	}
	for (size_t i = prism50TopEnd; i < LEN(prism50verts); i++) {
		glNormal3dv(prism50normals[i / 3]);
		glTexCoord2dv(prism50uv[i]);
		glVertex3dv(prism50verts[i]);
	}
	glEnd();
	if (alpha && lighting) {
		glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	}
	// OIT пока не придумали, соритровку граней оставлю на поотом (никогда)
	task2_draw_bottom();
	task2_draw_top();
}

void Render(double delta_time) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//натройка камеры и света
	//в этих функциях находятся OGLные функции
	//которые устанавливают параметры источника света
	//и моделвью матрицу, связанные с камерой.
	if (gl.isKeyPressed('0')) {
		reloadTexture(texId1, TEXTURE_1);
		reloadTexture(texId2, TEXTURE_2);
		reloadTexture(texId3, TEXTURE_3);
	}
	if (lightFollowsCamera ||
		gl.isKeyPressed('F')) /* если нажата F - свет из камеры */
		light.SetPosition(camera.x(), camera.y(), camera.z());
	camera.SetUpCamera();
	light.SetUpLight();

	//рисуем оси
	gl.DrawAxes();

	{
		// Режимы
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		//включаем режимы, в зависимости от нажания клавиш. см void switchModes(OpenGL *sender, KeyEventArg arg)
		if (lighting) glEnable(GL_LIGHTING);
		if (texturing) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0); //сбрасываем текущую текстуру
		}
		if (alpha) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}

	//=============НАСТРОЙКА МАТЕРИАЛА==============

	//настройка материала, все что рисуется ниже будет иметь этот метериал.
	//массивы с настройками материала
	float amb[] = { 0.2f, 0.2f, 0.1f, 1.f };
	float dif[] = { 0.8f, 0.8f, 0.8f, 1.f };
	float spec[] = { 0.9f, 0.8f, 0.3f, 1.f };
	float sh = 0.2f * 256;

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb); // фоновая
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif); // дифузная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); // зеркальная
	glMaterialf(GL_FRONT, GL_SHININESS, sh); // размер блика

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	if (smoothShade) glShadeModel(GL_SMOOTH); //закраска по Гуро
	else glShadeModel(GL_FLAT);
	//(GL_SMOOTH - плоская закраска)

	//============ РИСОВАТЬ ТУТ ==============

	//квадратик станкина
	//так как расчет освещения происходит только в вершинах
	// (закраска по Гуро)
	//то рисуем квадратик из более маленьких квадратиков
	glBindTexture(GL_TEXTURE_2D, texId1);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	double h = 0.025;
	for (double x = h; x <= 1; x += h)
		for (double y = h; y <= 1; y += h) {
			glColor3d(1, 1, 0);

			glTexCoord2d(x, y);
			glVertex2d(x, y);

			glTexCoord2d(x - h, y);
			glVertex2d(x - h, y);

			glTexCoord2d(x - h, y - h);
			glVertex2d(x - h, y - h);

			glTexCoord2d(x, y - h);
			glVertex2d(x, y - h);
		}
	glEnd();

	if (togglePrism) task2();
	else task1();

	// восстановить материал
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb); // фоновая
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif); // дифузная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); // зеркальная
	glMaterialf(GL_FRONT, GL_SHININESS, sh); // размер блика

	//===============================================

	//рисуем источник света
	light.DrawLightGizmo();

	//================Сообщение в верхнем левом углу=======================
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//переключаемся на матрицу проекции
	glMatrixMode(GL_PROJECTION);
	//сохраняем текущую матрицу проекции с перспективным преобразованием
	glPushMatrix();
	//загружаем единичную матрицу в матрицу проекции
	glLoadIdentity();

	//устанавливаем матрицу паралельной проекции
	glOrtho(0, gl.getWidth() - 1, 0, gl.getHeight() - 1, 0, 1);

	//переключаемся на моделвью матрицу
	glMatrixMode(GL_MODELVIEW);
	//сохраняем матрицу
	glPushMatrix();
	//сбразываем все трансформации и настройки камеры загрузкой единичной матрицы
	glLoadIdentity();

	//отрисованное тут будет визуалзироватся в 2д системе координат
	//нижний левый угол окна - точка (0,0)
	//верхний правый угол (ширина_окна - 1, высота_окна - 1)
	/*
	case 'L': lighting = !lighting;
	case 'T': texturing = !texturing;
	case 'A': alpha = !alpha;
	case 'S': smoothShade = !smoothShade;
	case 'U': lightFollowsCamera = !lightFollowsCamera;
	case 'C': insideCulling = !insideCulling;
	case 'N': showNormals = !showNormals;
	*/
	std::wstringstream ss;
	ss << std::fixed << std::setprecision(3);
	ss << "T - " << (texturing ? L"[вкл]выкл  " : L" вкл[выкл] ") <<
		L"текстура\n";
	ss << "L - " << (lighting ? L"[вкл]выкл  " : L" вкл[выкл] ") <<
		L"освещение\n";
	ss << "A - " << (alpha ? L"[вкл]выкл  " : L" вкл[выкл] ") <<
		L"альфа-наложение\n";
	ss << "S - " << (smoothShade ? L"[вкл]выкл  " : L" вкл[выкл] ") <<
		L"гладкое освещение\n";
	ss << "U - " << (lightFollowsCamera ? L"[вкл]выкл  " : L" вкл[выкл] ") <<
		L"свет следует за камерой\n";
	ss << "C - " << (insideCulling ? L"[вкл]выкл  " : L" вкл[выкл] ") <<
		L"удаление внутренних граней\n";
	ss << "N - " << (showNormals ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"нормали\n";
	ss << "; - " << (thickNormals ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"нормали толстой линией\n";
	ss << "\\ - " << (togglePrism ? L" [**]--    " : L"  **[--]   ") << L"переключить призму\n";
	ss << L"F - Свет из камеры\n";
	ss << L"G - двигать свет по горизонтали\n";
	ss << L"G+ЛКМ двигать свет по вертекали\n";
	ss << L"0 - перезагрузить текстуры\n";
	ss << L"Коорд. света: (" << std::setw(7) << light.x() << "," << std::setw(
		7) << light.y() << "," << std::setw(7) << light.z() << ")\n";
	ss << L"Коорд. камеры: (" << std::setw(7) << camera.x() << "," << std::setw(
		7) << camera.y() << "," << std::setw(7) << camera.z() << ")\n";
	ss << L"Параметры камеры: R=" << std::setw(7) << camera.distance() << ",fi1=" <<
		std::setw(7) << camera.fi1() << ",fi2=" << std::setw(7) << camera.fi2() << std::endl;
	ss << L"delta_time: " << std::setprecision(5) << delta_time << std::endl;

	text.setPosition(10, gl.getHeight() - 10 - 250);
	text.setText(ss.str().c_str());
	text.Draw();

	//восстанавливаем матрицу проекции на перспективу, которую сохраняли ранее.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
