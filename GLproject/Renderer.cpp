#include "Renderer.h"

void draw_center(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); /* R */
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.2f, 0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'x');

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f); /* G */
	glVertex3f(0.0f, 0.2f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.0f, 0.2f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'y');

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f); /* B */
	glVertex3f(0.0f, 0.0f, -0.2f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.0f, 0.0f, -0.2f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'z');
}

void idle() {
	static GLuint previousClock = glutGet(GLUT_ELAPSED_TIME);
	static GLuint currentClock = glutGet(GLUT_ELAPSED_TIME);
	static GLfloat deltaT;

	currentClock = glutGet(GLUT_ELAPSED_TIME);
	deltaT = currentClock - previousClock;
	if (deltaT < 1000.0 / 20.0) { return; }
	else { previousClock = currentClock; }

	//char buff[256];
	//sprintf_s(buff, "Frame Rate = %f", 1000.0 / deltaT);
	//frameRate = buff;

	glutPostRedisplay();
}

void close()
{
	glDeleteTextures(1, &dispBindIndex);
	glutLeaveMainLoop();
	CloseHandle(hMutex);
}

void add_quats(float q1[4], float q2[4], float dest[4])
{
	static int count = 0;
	float t1[4], t2[4], t3[4];
	float tf[4];

	vcopy(q1, t1);
	vscale(t1, q2[3]);

	vcopy(q2, t2);
	vscale(t2, q1[3]);

	vcross(q2, q1, t3);
	vadd(t1, t2, tf);
	vadd(t3, tf, tf);
	tf[3] = q1[3] * q2[3] - vdot(q1, q2);

	dest[0] = tf[0];
	dest[1] = tf[1];
	dest[2] = tf[2];
	dest[3] = tf[3];

	if (++count > RENORMCOUNT) {
		count = 0;
		normalize_quat(dest);
	}
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(58, (double)width / height, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	GLfloat spin_quat[4];
	float gain;
	gain = 2.0; /* trackball gain */

	if (drag_state == GLUT_DOWN)
	{
		if (button_state == GLUT_LEFT_BUTTON)
		{
			trackball(spin_quat,
				(gain * rot_x - 500) / 500,
				(500 - gain * rot_y) / 500,
				(gain * x - 500) / 500,
				(500 - gain * y) / 500);
			add_quats(spin_quat, quat, quat);
		}
		else if (button_state == GLUT_RIGHT_BUTTON)
		{
			t[0] -= (((float)trans_x - x) / 500);
			t[1] += (((float)trans_y - y) / 500);
		}
		else if (button_state == GLUT_MIDDLE_BUTTON)
			t[2] -= (((float)trans_z - y) / 500 * 4);
		else if (button_state == 3 || button_state == 4) // scroll
		{

		}
		//glutPostRedisplay();
	}

	rot_x = x;
	rot_y = y;

	trans_x = x;
	trans_y = y;
	trans_z = y;
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			rot_x = x;
			rot_y = y;

			//t[0] = t[0] + 1;


		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			trans_x = x;
			trans_y = y;
		}
		else if (button == GLUT_MIDDLE_BUTTON)
		{
			//trcon = trcon + 1;
			trans_z = y;
		}
		else if (button == 3 || button == 4)
		{
			const float sign = (static_cast<float>(button) - 3.5f) * 2.0f;
			t[2] -= sign * 500 * 0.00015f;
		}
	}

	drag_state = state;
	button_state = button;
}

void vzero(float* v)
{
	v[0] = 0.0f;
	v[1] = 0.0f;
	v[2] = 0.0f;
}

void vset(float* v, float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

void vsub(const float* src1, const float* src2, float* dst)
{
	dst[0] = src1[0] - src2[0];
	dst[1] = src1[1] - src2[1];
	dst[2] = src1[2] - src2[2];
}

void vcopy(const float* v1, float* v2)
{
	register int i;
	for (i = 0; i < 3; i++)
		v2[i] = v1[i];
}

void vcross(const float* v1, const float* v2, float* cross)
{
	float temp[3];

	temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
	vcopy(temp, cross);
}

float vlength(const float* v)
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void vscale(float* v, float div)
{
	v[0] *= div;
	v[1] *= div;
	v[2] *= div;
}

void vnormal(float* v)
{
	vscale(v, 1.0f / vlength(v));
}

float vdot(const float* v1, const float* v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void vadd(const float* src1, const float* src2, float* dst)
{
	dst[0] = src1[0] + src2[0];
	dst[1] = src1[1] + src2[1];
	dst[2] = src1[2] + src2[2];
}

void trackball(float q[4], float p1x, float p1y, float p2x, float p2y)
{
	float a[3]; /* Axis of rotation */
	float phi;  /* how much to rotate about axis */
	float p1[3], p2[3], d[3];
	float t;

	if (p1x == p2x && p1y == p2y) {
		/* Zero rotation */
		vzero(q);
		q[3] = 1.0;
		return;
	}

	/*
	 * First, figure out z-coordinates for projection of P1 and P2 to
	 * deformed sphere
	 */
	vset(p1, p1x, p1y, tb_project_to_sphere(TRACKBALLSIZE, p1x, p1y));
	vset(p2, p2x, p2y, tb_project_to_sphere(TRACKBALLSIZE, p2x, p2y));

	/*
	 *  Now, we want the cross product of P1 and P2
	 */
	vcross(p2, p1, a);

	/*
	 *  Figure out how much to rotate around that axis.
	 */
	vsub(p1, p2, d);
	t = vlength(d) / (2.0f * TRACKBALLSIZE);

	/*
	 * Avoid problems with out-of-control values...
	 */
	if (t > 1.0) t = 1.0;
	if (t < -1.0) t = -1.0;
	phi = 2.0f * asin(t);

	axis_to_quat(a, phi, q);
}

void axis_to_quat(float a[3], float phi, float q[4])
{
	vnormal(a);
	vcopy(a, q);
	vscale(q, sin(phi / 2.0f));
	q[3] = cos(phi / 2.0f);
}

float tb_project_to_sphere(float r, float x, float y)
{
	float d, t, z;

	d = sqrt(x * x + y * y);
	if (d < r * 0.70710678118654752440f) {    /* Inside sphere */
		z = sqrt(r * r - d * d);
	}
	else {           /* On hyperbola */
		t = r / 1.41421356237309504880f;
		z = t * t / d;
	}
	return z;
}

void normalize_quat(float q[4])
{
	int i;
	float mag;

	mag = (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	for (i = 0; i < 4; i++) q[i] /= mag;
}

void build_rotmatrix(float m[4][4], float q[4])
{
	m[0][0] = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
	m[0][1] = 2.0f * (q[0] * q[1] - q[2] * q[3]);
	m[0][2] = 2.0f * (q[2] * q[0] + q[1] * q[3]);
	m[0][3] = 0.0f;

	m[1][0] = 2.0f * (q[0] * q[1] + q[2] * q[3]);
	m[1][1] = 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
	m[1][2] = 2.0f * (q[1] * q[2] - q[0] * q[3]);
	m[1][3] = 0.0f;

	m[2][0] = 2.0f * (q[2] * q[0] - q[1] * q[3]);
	m[2][1] = 2.0f * (q[1] * q[2] + q[0] * q[3]);
	m[2][2] = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);
	m[2][3] = 0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

void InitializeWindow(int argc, char* argv[])
{
	// initialize glut settings
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(1000 / 1.5, 1000 / 1.5);

	glutInitWindowPosition(500, 300);

	dispWindowIndex = glutCreateWindow("3D Model");

	trackball(quat, 90.0, 0.0, 0.0, 0.0);

	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutCloseFunc(close);
	//GLuint image = load   ("./my_texture.bmp");

	//glBindTexture(1,)

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// bind textures
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	reshape(1000, 1000);

	/*glGenTextures(1, &dispBindIndex);
	glBindTexture(GL_TEXTURE_2D, dispBindIndex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 0.1, 200);
	glTranslatef(t[0], t[1], t[2] - 1.0f);
	glScalef(1, 1, 1);
	GLfloat m[4][4], m1[4][4];
	build_rotmatrix(m, quat);
	gluLookAt(0, 2.0, 2.0, 0, 0, 0, 0, 1.0, 0);

	GLfloat r, g, b;
	glMultMatrixf(&m[0][0]);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat diffuse0[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat ambient0[4] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat specular0[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light0_pos[4] = { 2.0, 2.0, 2.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);


	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.2);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);


	//빨간색 플라스틱과 유사한 재질을 다음과 같이 정의
	GLfloat mat_ambient[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat mat_diffuse[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat mat_specular[4] = { 0.8f, 0.6f, 0.6f, 1.0f };
	GLfloat mat_shininess = 32.0;

	// 폴리곤의 앞면의 재질을 설정 
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	// Flat Shading 추가
	glShadeModel(GL_FLAT);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	DrawObj(0);

	glEnd();

	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_QUADS);

	DrawObj(1);

	glEnd();


	glutSwapBuffers();
}


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  Function: DrawObj()

  Summary:

  Return: ...
-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void DrawObj(int idx)
{
	for (int jj = 0; jj < num_faces[idx]; jj = jj + 1)
	{
		glTexCoord2d(vcolorArr[idx][mymeshArr[idx][jj].T1 - 1].X, vcolorArr[idx][mymeshArr[idx][jj].T1 - 1].Y);
		glVertex3f(vertexArr[idx][mymeshArr[idx][jj].V1 - 1].X, vertexArr[idx][mymeshArr[idx][jj].V1 - 1].Y, vertexArr[idx][mymeshArr[idx][jj].V1 - 1].Z);
		glTexCoord2d(vcolorArr[idx][mymeshArr[idx][jj].T2 - 1].X, vcolorArr[idx][mymeshArr[idx][jj].T2 - 1].Y);
		glVertex3f(vertexArr[idx][mymeshArr[idx][jj].V2 - 1].X, vertexArr[idx][mymeshArr[idx][jj].V2 - 1].Y, vertexArr[idx][mymeshArr[idx][jj].V2 - 1].Z);
		glTexCoord2d(vcolorArr[idx][mymeshArr[idx][jj].T3 - 1].X, vcolorArr[idx][mymeshArr[idx][jj].T3 - 1].Y);
		glVertex3f(vertexArr[idx][mymeshArr[idx][jj].V3 - 1].X, vertexArr[idx][mymeshArr[idx][jj].V3 - 1].Y, vertexArr[idx][mymeshArr[idx][jj].V3 - 1].Z);
		glTexCoord2d(vcolorArr[idx][mymeshArr[idx][jj].T4 - 1].X, vcolorArr[idx][mymeshArr[idx][jj].T4 - 1].Y);
		if (mymeshArr[idx][jj].V4)
			glVertex3f(vertexArr[idx][mymeshArr[idx][jj].V4 - 1].X, vertexArr[idx][mymeshArr[idx][jj].V4 - 1].Y, vertexArr[idx][mymeshArr[idx][jj].V4 - 1].Z);
	}
}


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  Function: LoadObj(const char* filepath)

  Summary: read v, vt, f
		   & count vertex, find vertex range
		   & save to vertex, vertex_color, mymesh structure

  Return: ...
-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void NewLoadObj(int idx, const char* filepath, int move_x, int move_y, int move_z)
{
	FILE* fp;
	fp = fopen(filepath, "r");

	num_vertices = 0;
	num_faces[idx] = 0;

	x_min = 100000, x_max = -100000;
	y_min = 100000, y_max = -100000;
	z_min = 100000, z_max = -100000;

	int tmp;
	int count = 0;
	int num = 0;
	char ch;
	float x, y, z;
	float tmp_x, tmp_y, tmp_z;
	float x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;

	int idx_vtx = 0;
	int idx_vtxc = 0;
	int idx_mesh = 0;

	while (1)
	{
		char lineHeader2[128];
		// read the first word of the line
		int res2 = fscanf(fp, "%s", lineHeader2);
		if (res2 == EOF)
			break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader2, "v") == 0)
		{
			count = fscanf(fp, "%f %f %f /n", &x, &y, &z);
			if (count == 3)
			{
				if (x < x_min)
					x_min = x;
				if (x > x_max)
					x_max = x;
				if (y < y_min)
					y_min = y;
				if (y > y_max)
					y_max = y;
				if (z < z_min)
					z_min = z;
				if (z > z_max)
					z_max = z;

				num_vertices++;
			}
		}
		/*else
			break;*/
	}
	fclose(fp);

	myscale = max(max(x_max - x_min, y_max - y_min), z_max - z_min);

	fp = fopen(filepath, "r");

	while (1)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(fp, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "v") == 0)
		{
			count = fscanf(fp, "%f %f %f /n", &x, &y, &z);
			if (count == 3)
			{
				tmp_x = x - x_min;
				tmp_y = y - y_min;
				tmp_z = z - z_min;
				vertexArr[idx][idx_vtx].X = tmp_x / myscale + move_x;
				vertexArr[idx][idx_vtx].Y = tmp_y / myscale + move_y;
				vertexArr[idx][idx_vtx].Z = tmp_z / myscale + move_z;

				idx_vtx++;
			}
			if (idx_vtx) {};
		}

		else if (strcmp(lineHeader, "vt") == 0)
		{
			count = fscanf(fp, "%f %f /n", &x, &y);
			if (count == 2)
			{
				vcolorArr[idx][idx_vtxc].X = x;
				vcolorArr[idx][idx_vtxc].Y = y;
				vcolorArr[idx][idx_vtxc].Z = 0.000000;

				idx_vtxc++;
			}
			if (idx_vtxc) {};
		}

		else if (strcmp(lineHeader, "f") == 0)
		{
			// v/vt/vn
			x4 = 0, y4 = 0, z4 = 0;
			count = fscanf(fp, "%f/%f/%f %f/%f/%f %f/%f/%f %f/%f/%f /n", &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3, &x4, &y4, &z4);

			if (count >= 9)
			{
				mymeshArr[idx][idx_mesh].V1 = x1;
				mymeshArr[idx][idx_mesh].V2 = x2;
				mymeshArr[idx][idx_mesh].V3 = x3;
				mymeshArr[idx][idx_mesh].V4 = x4;
				mymeshArr[idx][idx_mesh].T1 = y1;
				mymeshArr[idx][idx_mesh].T2 = y2;
				mymeshArr[idx][idx_mesh].T3 = y3;
				mymeshArr[idx][idx_mesh].T4 = y4;

				idx_mesh++;
			}
		}
	}
	num_faces[idx] = idx_mesh;

	if (num_faces) {};

	fclose(fp);
}


int main(int argc, char* argv[])
{
	numobject = 2;

	vertexArr = new Vertex * [numobject];
	for (int i = 0; i < numobject; i++) {
		vertexArr[i] = new Vertex[100000];
	}
	vcolorArr = new Vertex * [numobject];
	for (int i = 0; i < numobject; i++) {
		vcolorArr[i] = new Vertex[100000];
	}
	mymeshArr = new MMesh * [numobject];
	for (int i = 0; i < numobject; i++) {
		mymeshArr[i] = new MMesh[100000];
	}

	int i, j, k = 0;
	FILE* f = fopen("assets/nightmare/spiral_hill/spiral_hill_texture.bmp", "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
	// extract image height and width from header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];

	int size = 3 * width * height;
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
	fclose(f);
	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
		{
			mytexels[0][j][i][0] = data[k * 3 + 2];
			mytexels[0][j][i][1] = data[k * 3 + 1];
			mytexels[0][j][i][2] = data[k * 3];
			k++;
		}

	i, j, k = 0;
	f = fopen("assets/apple/applet.bmp", "rb");
	unsigned char info2[54];
	fread(info2, sizeof(unsigned char), 54, f); // read the 54-byte header
	// extract image height and width from header
	width = *(int*)&info2[18];
	height = *(int*)&info2[22];

	size = 3 * width * height;
	unsigned char* data2 = new unsigned char[size]; // allocate 3 bytes per pixel
	fread(data2, sizeof(unsigned char), size, f); // read the rest of the data at once
	fclose(f);
	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
		{
			mytexels[1][j][i][0] = data2[k * 3 + 2];
			mytexels[1][j][i][1] = data2[k * 3 + 1];
			mytexels[1][j][i][2] = data2[k * 3];
			k++;
		}

	NewLoadObj(0, "assets/nightmare/spiral_hill/spiral_hill.obj", 1, 0, 0);
	NewLoadObj(1, "assets/apple/bigapple.obj", 0, 0, 0);


	InitializeWindow(argc, argv);

	display();

	glutMainLoop();

	for (int i = 0; i < numobject; i++) {
		delete[] vertexArr[i];
	}
	delete[] vertexArr;
	for (int i = 0; i < numobject; i++) {
		delete[] vcolorArr[i];
	}
	delete[] vcolorArr;
	for (int i = 0; i < numobject; i++) {
		delete[] mymeshArr[i];
	}
	delete[] mymeshArr;

	return 0;
}