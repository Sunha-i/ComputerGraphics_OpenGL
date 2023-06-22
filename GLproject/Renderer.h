#pragma once

#include <math.h>
#include <iostream>
#include <fstream>
#include "gl\freeglut.h"		// OpenGL header files
#include <list>
//#define TIME_CHECK_
#define DEPTH_CALIB_
#pragma warning(disable:4996)
#define BLOCK 4
#define READ_SIZE 110404//5060//110404
#define scale 30
//#define TEST 14989
//#define STATIC_

using namespace std;

struct Vertex {
	float X;
	float Y;
	float Z;
	int index_1;
	int index_2;
	int index_3;
};

struct MMesh {
	int V1;
	int V2;
	int V3;
	int V4;
	int T1;
	int T2;
	int T3;
	int T4;
};

// variables for GUI
const float TRACKBALLSIZE = 0.8f;
const int RENORMCOUNT = 97;
//const int width = KinectBasic::nColorWidth;
//const int height = KinectBasic::nColorHeight;

GLint drag_state = 0;
GLint button_state = 0;

GLint rot_x = 0;
GLint rot_y = 0;
GLint trans_x = 0;
GLint trans_y = 0;
GLint trans_z = 0;

//GLubyte mytexels[2048][2048][3];

int add_depth_flag = 0;
int model_flag = 0;
int depth_display_flag = 0;
int geodesic_skel[23][5] = { 0 };
int trcon = 0;
float zmin = 100000, zmax = -100000;

int side_status[50] = { 0 };

float quat[4] = { 0 };
float t[3] = { 0 };

Vertex skt[23];
BOOLEAN bTracked = false;
bool checkt = false;
Vertex* vertex;
Vertex* vertex_color;
MMesh* mymesh;

bool recheck;

// variables for display OpenGL based point viewer
int dispWindowIndex = 0;
GLuint dispBindIndex = 0;
const float dispPointSize = 2.0f;

// variables for display text
string dispString = "";
const string dispStringInit = "Depth Threshold: D\nInfrared Threshold: I\nNonlocal Means Filter: N\nPick BodyIndex: P\nAccumulate Mode: A\nSelect Mode: C,B(select)\nSave: S\nReset View: R\nQuit: ESC";
string frameRate;

HANDLE hMutex;
//KinectBasic kinect;

// functions for GUIs
void InitializeWindow(int argc, char* argv[]);

// high-level functions for GUI
void draw_center();
void idle();
void display();
void close();
void special(int, int, int) {}
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void reshape(int, int);
void motion(int, int);


// basic functions for computation/GUI
// trackball codes were imported from those of Gavin Bell
// which appeared in SIGGRAPH '88
void vzero(float*);
void vset(float*, float, float, float);
void vsub(const float*, const float*, float*);
void vcopy(const float*, float*);
void vcross(const float* v1, const float* v2, float* cross);
float vlength(const float* v);
void vscale(float* v, float div);
void vnormal(float* v);
float vdot(const float* v1, const float* v2);
void vadd(const float* src1, const float* src2, float* dst);

void trackball(float q[4], float, float, float, float);
//void add_quats(float*, float*, float*);
void axis_to_quat(float a[3], float phi, float q[4]);
void normalize_quat(float q[4]);
float tb_project_to_sphere(float, float, float);
void build_rotmatrix(float m[4][4], float q[4]);
void Reader();
//void DrawObj();
void DrawMeshObj();
void Setskt();

//CameraSpacePoint m_SpacePoint[JointType::JointType_Count];
void Track();



// myheader
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))

float x_size;	// x_max - x_min
float y_size;	// y_max - y_min
float z_size;	// z_max - z_min
float x_min = 100000, x_max = -100000;
float y_min = 100000, y_max = -100000;
float z_min = 100000, z_max = -100000;

int num_vertices;
int num_faces[20];

int numobject;
float normalize;	// vertex�� ������ ����ȭ (box�� max line�� ũ�Ⱑ 1�� �ǵ���, ��ġ�� ����ȵ�)
void DrawObj(int idx);
void LoadObj(int idx, const char* bmppath, const char* objpath, float myscale = 1, float move_x = 0, float move_y = 0, float move_z = 0);
void SetTexture(int idx, const char* filepath);

// �ϴ� ����ȭ�� �ϸ鼭 origin���� �����ϱ�
// �̰� fitting box ���� ũ��

Vertex* vertex2;
Vertex* vertex_color2;
MMesh* mymesh2;

Vertex** vertexArr;
Vertex** vcolorArr;
MMesh** mymeshArr;
GLubyte mytexels[20][2048][2048][3];