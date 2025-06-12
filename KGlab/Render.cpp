/* vim: set fdm=indent : */

#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>
#include <algorithm>
#include "stb_image.h"

#include "Render.h"
#include "GUItextRectangle.h"
#include "vec.h"
#include "Curves.h"
#include "lab2.h"
#include "lab3.h"
#include "lab3tr.h"

#define LEN(x) (sizeof((x))/sizeof(*(x)))

#include "MyOGL.h"
extern OpenGL gl;
#include "Light.h"
Light light;
#include "Camera.h"
Camera camera;
GuiTextRectangle text;

double total_time = 0;
bool texturing = true;
bool lighting = true;
bool alpha = true;
bool smoothShade = true;
bool lightFollowsCamera = true;
bool insideCulling = false;
bool showNormals = false;
bool thickNormals = false;
bool togglePrism = true;
GLuint texId1, texId2, texId3;

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

GLuint prepareTexture(char const *path) {
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

void reloadTexture(GLuint id, char const *path) {
	/* Загрузка текстуры */
	int x, y, n;
	unsigned char *data = stbi_load(path, &x, &y, &n, 4);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
}

//выполняется один раз перед первым рендером
void initRender() {
	prepare_lab2();
	prepare_lab3();
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

void draw_camera() {
	//натройка камеры и света
	//в этих функциях находятся OGLные функции
	//которые устанавливают параметры источника света
	//и моделвью матрицу, связанные с камерой.
	if (gl.isKeyPressed('0')) {
		reloadTexture(texId1, TEXTURE_1);
		reloadTexture(texId2, TEXTURE_2);
		reloadTexture(texId3, TEXTURE_3);
	}
	/* если нажата F - свет из камеры */
	if (lightFollowsCamera || gl.isKeyPressed('F'))
		light.SetPosition(camera.x(), camera.y(), camera.z());
	camera.SetUpCamera();
	light.SetUpLight();
}

void set_params() {
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

void set_material() {
	//=============НАСТРОЙКА МАТЕРИАЛА==a============

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
}

void draw_stankin_square() {
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
}

void draw_debug_screen(double delta_time) {
	//================Сообщение в верхнем левом углу=======================
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION); /* переключаемся на матрицу проекции */
	glPushMatrix(); /* сохраняем текущую матрицу проекции с перспективным преобразованием* */
	glLoadIdentity(); /* загружаем единичную матрицу в матрицу проекции */

	//устанавливаем матрицу паралельной проекции
	glOrtho(0, gl.getWidth() - 1, 0, gl.getHeight() - 1, 0, 1);

	glMatrixMode(GL_MODELVIEW); /* переключаемся на моделвью матрицу */
	glPushMatrix(); /* сохраняем матрицу */
	glLoadIdentity(); /* сбразываем все трансформации и настройки камеры */

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
	// *INDENT-OFF*
	std::wstringstream ss;
	ss << std::fixed << std::setprecision(3);
	ss << "T - " << (texturing ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"текстура\n";
	ss << "L - " << (lighting ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"освещение\n";
	ss << "A - " << (alpha ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"альфа-наложение\n";
	ss << "S - " << (smoothShade ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"гладкое освещение\n";
	ss << "U - " << (lightFollowsCamera ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"свет следует за камерой\n";
	ss << "C - " << (insideCulling ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"удаление внутренних граней\n";
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
	// *INDENT-ON*

	text.setPosition(10, gl.getHeight() - 10 - 250);
	text.setText(ss.str().c_str());
	text.Draw();

	//восстанавливаем матрицу проекции на перспективу, которую сохраняли ранее.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Render(double delta_time) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	draw_camera();
	gl.DrawAxes();
	set_params();
	set_material();

	//============ РИСОВАТЬ ТУТ ==============
	// draw_stankin_square();
	// if (togglePrism) task2();
	// else task1();
	// l3task30();
	l3task40();
	// l3tr();

	//===============================================
	set_material(); /* Восстановливаем материал */
	light.DrawLightGizmo(); /* рисуем источник света */
	draw_debug_screen(delta_time);
	total_time += delta_time;
}
