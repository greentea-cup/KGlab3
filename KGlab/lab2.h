#ifndef LAB2_H
#define LAB2_H
#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include "vec.h"

#define LEN(x) (sizeof((x))/sizeof(*(x)))

extern bool texturing;
extern bool lighting;
extern bool alpha;
extern bool smoothShade;
extern bool lightFollowsCamera;
extern bool insideCulling;
extern bool showNormals;
extern bool thickNormals;
extern bool togglePrism;
extern GLuint texId1, texId2, texId3;

#define HEIGHT 3.0
#define NEXTRA1 512
#define NEXTRA2 512
#define TEXTURE_1 "texture_1.png"
#define TEXTURE_2 "texture_2.png"
#define TEXTURE_3 "texture_3.png"
#define	UV_30_ZERO {-7., -8., 0.}
#define UV_50_ZERO {-7., -8., 0.}
inline double const UV_30_SCALE = 1. / 15.;
inline double const UV_50_SCALE = 1. / 18.;
#define UV_50_START_BOTTOM 36
#define UV_50_END_BOTTOM 54
#define UV_50_START_TOP 54
#define UV_50_END_TOP 72
inline size_t const UV_50_LEN_BOTTOM = UV_50_END_BOTTOM - UV_50_START_BOTTOM;
inline size_t const UV_50_LEN_TOP = UV_50_END_TOP - UV_50_START_TOP;
inline double const SIDE_UV_SCALE = 1. / HEIGHT;

enum winding_order {
	WND_CCW = 0, /* Против часовой */
	WND_CW = 1, /* По часовой */
};

vec3 circle_center_xy(vec3 p1, vec3 p2, vec3 p3);
void fill_arc_points_xy(vec3 *data, size_t npoints, winding_order cw, vec3 a, vec3 b, double distance_xy, vec3 center);
void calc_normals(vec3 const *verts, size_t npoints, vec3 *normals);
void calc_uv(vec3 const *verts, size_t npoints, vec2 *uv, vec3 zero, double scale);
void calc_uv_xrev(vec3 const *verts, size_t npoints, vec2 *uv, vec3 zero, double scale);
void reverse_uv_x(vec2 *uv, size_t npoints);
double mind(double a, double b, double c);
double maxd(double a, double b, double c);
extern size_t const prism50len;
extern size_t prism50BottomStart, prism50BottomEnd, prism50TopStart, prism50TopEnd;
void fill_prism50(void);
void gen_side_uv(vec3 const *verts, size_t len, vec2 *uv);

void draw_normals(vec3 const *verts, vec3 const *normals, size_t npoints);
void prepare_lab2(void);
void l2task1();
void l2task2_draw_bottom();
void l2task2_draw_top();
void l2task2();

#endif
