#include "Train.h"
#include "glut.h"
#include <math.h>

Train::Train(double x, double y, double z, double dx, double dy, double dz, double speed)
{
	cx = x;
	cy = y;
	cz = z;
	dirx = dx;
	diry = dy;
	dirz = dz;
	this->speed = speed;
}

// kind of ring
void Train::DrawTyer(int n, double outer, double inner, int jump)
{
	double alpha, teta = 2 * PI / n;
	double x, z;

	for (alpha = 0; alpha < 2 * PI; alpha += jump * teta)
	{
		glBegin(GL_POLYGON);
		x = outer * sin(alpha);
		z = outer * cos(alpha);
		glVertex3d(x, 0, z); // 1
		x = inner * sin(alpha);
		z = inner * cos(alpha);
		glVertex3d(x, 0, z); // 2
		x = inner * sin(alpha + teta);
		z = inner * cos(alpha + teta);
		glVertex3d(x, 0, z); // 3
		x = outer * sin(alpha + teta);
		z = outer * cos(alpha + teta);
		glVertex3d(x, 0, z); // 4
		glEnd();
	}
}

void Train::DrawCylinder(int n)
{
	double alpha, teta = 2 * PI / n;

	for (alpha = 0; alpha <= 2 * PI; alpha += teta)
	{
		// defines one side

		glBegin(GL_POLYGON);
		glVertex3d(sin(alpha), 1, cos(alpha)); // vertex 1
		glVertex3d(sin(alpha + teta), 1, cos(alpha + teta)); // vertex 2
		glVertex3d(sin(alpha + teta), 0, cos(alpha + teta)); // vertex 3
		glVertex3d(sin(alpha), 0, cos(alpha)); // vertex 4
		glEnd();
	}
}

void Train::DrawConus(int n)
{
	double alpha, teta = 2 * PI / n;

	glColor3d(1, 1, 1);
	for (alpha = 0; alpha <= 2 * PI; alpha += teta)
	{
		// defines one side

		glBegin(GL_POLYGON);
		glVertex3d(0, 1, 0); // vertex 1
		glVertex3d(sin(alpha + teta), 0, cos(alpha + teta)); // vertex 2
		glVertex3d(sin(alpha), 0, cos(alpha)); // vertex 3
		glEnd();
	}
}

void Train::DrawLitCylinder1(int n, double topr, double bottomr)
{
	double alpha, teta = 2 * PI / n, h, len;

	for (alpha = 0; alpha <= 2 * PI; alpha += teta)
	{
		h = (bottomr - topr) * bottomr;

		// defines one side
		glBegin(GL_POLYGON);
		glNormal3d(topr * sin(alpha), h, topr * cos(alpha));
		glVertex3d(topr * sin(alpha), 1, topr * cos(alpha)); // vertex 1
		glNormal3d(topr * sin(alpha + teta), h, topr * cos(alpha + teta));
		glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // vertex 2
		glNormal3d(bottomr * sin(alpha + teta), h, bottomr * cos(alpha + teta));
		glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // vertex 3
		glNormal3d(bottomr * sin(alpha), h, bottomr * cos(alpha));
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // vertex 4
		glEnd();
	}
}

void Train::DrawLitSphere(int n, int sl)
{
	double beta, gamma = PI / sl;


	for (beta = -PI / 2; beta <= PI / 2; beta += gamma)
	{
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta + gamma) - sin(beta), 1);
		DrawLitCylinder1(n, cos(beta + gamma), cos(beta));
		glPopMatrix();
	}
}

void Train::DrawWheel(double offset)
{
	glPushMatrix();
	glRotated(-50 * offset, 0, 1, 0);
	glPushMatrix();
	glColor3d(0.2, 0.2, 0.2);
	DrawCylinder(50);
	glScaled(0.7, 1, 0.7);
	DrawCylinder(50);
	glPopMatrix();
	glColor3d(0.4, 0.4, 0.4);
	DrawTyer(50, 1, 0.7, 1);
	glColor3d(0.8, 0.8, 0.8);
	DrawTyer(20, 0.7, 0, 2);
	glPushMatrix();
	glTranslated(0, 1, 0);
	glColor3d(0.4, 0.4, 0.4);
	DrawTyer(50, 1, 0.7, 1);
	glColor3d(0.8, 0.8, 0.8);
	DrawTyer(20, 0.7, 0, 2);
	glPopMatrix();
	glPopMatrix();
}

void Train::DrawWindows(bool right, bool left, bool front, bool rear, double r, double g, double b, double alpha)
{
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(r, g, b, alpha);

	// right
	if (right) {
		glBegin(GL_POLYGON);
		glVertex3d(-3.8, 7, 2.7);
		glVertex3d(-7, 7, 2.7);
		glVertex3d(-7, 4.5, 2.7);
		glVertex3d(-3.8, 4.5, 2.7);
		glEnd();
	}

	// left
	if (left) {
		glBegin(GL_POLYGON);
		glVertex3d(-3.8, 7, -2.7);
		glVertex3d(-7, 7, -2.7);
		glVertex3d(-7, 4.5, -2.7);
		glVertex3d(-3.8, 4.5, -2.7);
		glEnd();
	}

	// front
	if (front) {
		glBegin(GL_POLYGON);
		glVertex3d(-7, 7, -2.7);
		glVertex3d(-7, 7, 2.7);
		glVertex3d(-7, 4.5, 2.7);
		glVertex3d(-7, 4.5, -2.7);
		glEnd();
	}

	// rear
	if (rear) {
		glBegin(GL_POLYGON);
		glVertex3d(-3.8, 7, -2.7);
		glVertex3d(-3.8, 7, 2.7);
		glVertex3d(-3.8, 4.5, 2.7);
		glVertex3d(-3.8, 4.5, -2.7);
		glEnd();
	}

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

void Train::DrawLowerBody()
{
	glPushMatrix();
	glTranslated(0, 0.3, 0);
	glRotated(90, 0, 0, 1);
	glScaled(1, 1, 4);
	glRotated(45, 0, 1, 0);
	glTranslated(0, -7.5, 0);
	glScaled(1, 15, 1);
	DrawLitCylinder1(4, 1, 1);
	glPopMatrix();
}

void Train::DrawPanels()
{
	// front panel
	glColor3d(1, 1, 1);
	glBegin(GL_POLYGON);
	glVertex3d(7.5, 1.0, -2.9);
	glVertex3d(7.5, 1.0, 2.9);
	glVertex3d(7.5, -0.4, 2.9);
	glVertex3d(7.5, -0.4, -2.9);
	glEnd();

	// rear panel
	glBegin(GL_POLYGON);
	glVertex3d(-7.5, 1.0, -2.9);
	glVertex3d(-7.5, 1.0, 2.9);
	glVertex3d(-7.5, -0.4, 2.9);
	glVertex3d(-7.5, -0.4, -2.9);
	glEnd();
}

void Train::DrawTrainWheels(double offset)
{
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslated(6, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(6, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-6, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-6, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(3.5, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(3.5, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-3.5, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-3.5, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.25, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.25, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-1.25, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-1.25, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glEnable(GL_LIGHTING);
}

void Train::DrawTrainDriverArea()
{
	glPushMatrix();
	glTranslated(-4, 4, -2.6);
	glScaled(0.07, 1, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-4, 4, 2.6);
	glScaled(0.07, 1, 0.07);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-7, 4, -2.6);
	glScaled(0.07, 1, 0.07);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-7, 4, 2.6);
	glScaled(0.07, 1, 0.07);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-5.3, 2.8, -2.6);
	glScaled(0.5, 0.6, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-5.3, 2.8, 2.6);
	glScaled(0.5, 0.6, 0.07);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-7, 2.8, 0);
	glScaled(0.07, 0.6, 0.9);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-4, 2.8, 0);
	glScaled(0.07, 0.6, 0.9);
	glutSolidCube(6);
	glPopMatrix();

	// roof
	glPushMatrix();
	glTranslated(-5.5, 7.1, 0);
	glScaled(0.56, 0.03, 0.9);
	glutSolidCube(6);
	glPopMatrix();
}

void Train::DrawTrainEngineArea()
{
	glPushMatrix();
	glTranslated(-1, 3, 0);
	glRotated(-90, 0, 0, 1);
	glScaled(1.8, 3, 1.8);
	glPushMatrix();
	glTranslated(0, -0.92, 0);
	glScaled(1, 3.7, 1);
	DrawLitCylinder1(30, 1, 1);
	glPopMatrix();
	glScaled(1.1, 1, 1.1);
	glTranslated(0, 2.8, 0);
	DrawConus(50);
	glPopMatrix();

	// front lightning
	glPushMatrix();
	glTranslated(6, 5.5, 0);
	glScaled(0.3, 1, 0.3);
	glutSolidCube(2);
	glTranslated(1.4, 0.3, 0.5);
	glScaled(0.5, 0.5, 0.8);

	// set material: red plastic
	SetMaterial(0, 120);

	glutSolidSphere(1.5, 3, 3);
	glPopMatrix();

	// set material: metal
	SetMaterial(1, 100);

	// chimney
	glPushMatrix();
	glTranslated(5, 4.8, 0);
	glScaled(0.3, 1.5, 0.3);
	DrawLitCylinder1(30, 1, 1);
	glTranslated(0, 1, 0);
	glScaled(0.5, 0.5, 0.5);
	DrawLitCylinder1(30, 4, 2);
	glTranslated(0, 1, 0);
	DrawLitCylinder1(30, 3, 3.7);
	glTranslated(0, 1, 0);
	glPopMatrix();

	// other components
	glPushMatrix();
	glTranslated(3, 4.8, 0);
	glScaled(0.3, 1.5, 0.3);
	DrawLitCylinder1(30, 1, 1);
	glTranslated(0, 1, 0);
	glScaled(1, 0.2, 1);
	DrawLitSphere(30, 30);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.7, 4.8, 0);
	glScaled(0.1, 0.5, 0.1);
	DrawLitCylinder1(30, 1, 1);
	glTranslated(0, 1, 0);
	glScaled(1, 0.2, 1);
	DrawLitSphere(30, 30);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.2, 4.8, 0);
	glScaled(0.1, 1, 0.1);
	DrawLitCylinder1(30, 1, 1);
	glTranslated(0, 1, 0);
	glScaled(1, 0.1, 1);
	DrawLitSphere(30, 30);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.7, 4.8, 0);
	glScaled(0.1, 0.7, 0.1);
	DrawLitCylinder1(30, 1, 1);
	glTranslated(0, 1, 0);
	glScaled(1, 0.143, 1);
	DrawLitSphere(30, 30);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-1, 4.8, 0);
	glPushMatrix();
	glScaled(0.8, 1.5, 0.8);
	DrawLitCylinder1(30, 1, 1);
	glPopMatrix();
	glTranslated(0, 1.55, 0);
	glScaled(0.804375, 0.858, 0.804375);
	DrawLitSphere(30, 30);
	glPopMatrix();

	// front panel
	glColor3d(1, 1, 1);
	glBegin(GL_POLYGON);
	glVertex3d(7.5, 1.0, -2.9);
	glVertex3d(7.5, 1.0, 2.9);
	glVertex3d(7.5, -0.4, 2.9);
	glVertex3d(7.5, -0.4, -2.9);
	glEnd();

	// rear panel
	glBegin(GL_POLYGON);
	glVertex3d(-7.5, 1.0, -2.9);
	glVertex3d(-7.5, 1.0, 2.9);
	glVertex3d(-7.5, -0.4, 2.9);
	glVertex3d(-7.5, -0.4, -2.9);
	glEnd();
}

void Train::DrawTrainChimneySmoke()
{
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();

	double i = 0;
	double smokex = 0;
	double smokey = 0;
	double sizeScale = 1;

	glColor4d(0.3, 0.3, 0.3, 0.8);
	glTranslated(5, 9, 0);

	while (sizeScale >= 0.32)
	{
		i++;
		sizeScale -= i / 30;
		smokex -= 1.4;
		smokey += 1.2;

		if (i > 5) {
			smokex -= 1.3;
			smokey += 1.2;
		}

		glPushMatrix();
		glTranslated(smokex, smokey, 0);
		glScaled(sizeScale, sizeScale, sizeScale);
		glutSolidSphere(2, 30, 30);
		glPopMatrix();

		if (i > 5) {
			glPushMatrix();
			glTranslated(0, 0, 1);
			glScaled(sizeScale, sizeScale, sizeScale);
			glutSolidSphere(2, 30, 30);
			glPopMatrix();
			glPushMatrix();
			glTranslated(0, 0, -2);
			glScaled(sizeScale, sizeScale, sizeScale);
			glutSolidSphere(2, 30, 30);
			glPopMatrix();
		}
	}
	glPopMatrix();
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

void Train::DrawTrainUpperBody()
{
	DrawTrainDriverArea();
	DrawTrainEngineArea();
	DrawTrainChimneySmoke();
}

void Train::DrawTrainConnections(bool isRearConnected)
{
	if (isRearConnected)
	{
		glPushMatrix();
		glTranslated(-7.3, 0.2, 0);
		glScaled(0.8, 0.5, 1.5);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-8.1, 0.4, 0.2);
		glScaled(1.8, 0.3, 0.3);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-8.8, 0.4, 0.2);
		glScaled(0.3, 0.3, 0.3);
		glRotated(90, 1, 0, 0);
		glutSolidCone(0.4, 3.5, 30, 30);
		glPopMatrix();
	}
}

void Train::DrawCartWheels(double offset)
{
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslated(6, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(6, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-6, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-6, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(2, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(2, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-2, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-2, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel(offset);
	glPopMatrix();

	glEnable(GL_LIGHTING);
}

void Train::DrawCartUpperBody()
{
	glPushMatrix();
	glTranslated(6.5, 4, -2.6);
	glScaled(0.07, 1, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(6.5, 4, 2.6);
	glScaled(0.07, 1, 0.07);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(3.3, 4, -2.6);
	glScaled(0.07, 1, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(3.3, 4, 2.6);
	glScaled(0.07, 1, 0.07);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.1, 4, -2.6);
	glScaled(0.07, 1, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.1, 4, 2.6);
	glScaled(0.07, 1, 0.07);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-3.5, 4, -2.6);
	glScaled(0.07, 1, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-3.5, 4, 2.6);
	glScaled(0.07, 1, 0.07);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-6.7, 4, -2.6);
	glScaled(0.07, 1, 0.07);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-6.7, 4, 2.6);
	glScaled(0.07, 1, 0.07);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(4.8, 2.8, -2.6);
	glScaled(0.5, 0.6, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.6, 2.8, -2.6);
	glScaled(0.5, 0.6, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-1.8, 2.8, -2.6);
	glScaled(0.5, 0.6, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-4.8, 2.8, -2.6);
	glScaled(0.5, 0.6, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();
	glPushMatrix();
	glTranslated(4.8, 2.8, 2.6);
	glScaled(0.5, 0.6, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.6, 2.8, 2.6);
	glScaled(0.5, 0.6, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-1.8, 2.8, 2.6);
	glScaled(0.5, 0.6, 0.07);
	glColor3d(0, 0, 0);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-4.8, 2.8, 2.6);
	glScaled(0.5, 0.6, 0.07);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-6.5, 2.8, 0);
	glScaled(0.07, 0.6, 0.9);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(6.5, 2.8, 0);
	glScaled(0.07, 0.6, 0.9);
	glutSolidCube(6);
	glPopMatrix();
}

void Train::DrawCartRoof()
{
	glPushMatrix();
	glTranslated(-0.1, 7.1, 0);
	glScaled(2.3, 0.03, 0.9);
	glutSolidCube(6);
	glPopMatrix();
}

void Train::DrawCartConnections(bool isFrontConnected, bool isRearConnected)
{
	if (isFrontConnected)
	{
		glPushMatrix();
		glTranslated(7.3, 0.2, 0);
		glScaled(1, 0.5, 1.5);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(8.1, 0, 0);
		glScaled(1.5, 0.3, 0.3);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(9.2, 0, 0);
		glScaled(0.3, 0.3, 0.3);
		glRotated(90, 1, 0, 0);
		glutSolidTorus(0.5, 1, 10, 10);
		glPopMatrix();
	}
	if (isRearConnected)
	{
		glPushMatrix();
		glTranslated(-7.3, 0.2, 0);
		glScaled(0.8, 0.5, 1.5);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-8.1, 0.4, 0.2);
		glScaled(1.8, 0.3, 0.3);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-8.8, 0.4, 0.2);
		glScaled(0.3, 0.3, 0.3);
		glRotated(90, 1, 0, 0);
		glutSolidCone(0.4, 3.5, 30, 30);
		glPopMatrix();
	}
}

void Train::DrawCartWindows(double windowColorR, double windowColorG, double windowColorB, double alpha)
{
	glPushMatrix();
	DrawWindows(true, true, true, false, windowColorR, windowColorG, windowColorB, alpha);
	glTranslated(3.81, 0, 0);
	DrawWindows(true, true, false, false, windowColorR, windowColorG, windowColorB, alpha);
	glTranslated(3.31, 0, 0);
	DrawWindows(true, true, false, false, windowColorR, windowColorG, windowColorB, alpha);
	glTranslated(3.32, 0, 0);
	DrawWindows(true, true, false, true, windowColorR, windowColorG, windowColorB, alpha);
	glPopMatrix();
}

void Train::DrawCart(double offset, int cartMaterial, int cartShininess,
	double windowColorR, double windowColorG, double windowColorB, double alpha, bool isFrontConnected, bool isRearConnected)
{
	DrawCartWheels(offset);
	SetMaterial(cartMaterial, cartShininess);
	DrawLowerBody();
	DrawCartUpperBody();
	DrawCartRoof();
	DrawPanels();
	DrawCartConnections(isFrontConnected, isRearConnected);
	DrawCartWindows(windowColorR, windowColorG, windowColorB, alpha);
}

void Train::DrawTrain(double offset, int trainMaterial, int trainShininess,
	double windowColorR, double windowColorG, double windowColorB, double alpha, bool isRearConnected)
{
	DrawTrainWheels(offset);
	SetMaterial(trainMaterial, trainShininess);
	DrawLowerBody();
	DrawTrainUpperBody();
	DrawTrainConnections(isRearConnected);
	DrawWindows(true, true, true, true, windowColorR, windowColorG, windowColorB, alpha);
}

void Train::SetMaterial(int materialNumber, int shininess)
{
	switch (materialNumber)
	{
	// set material: red plastic
	case 0:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb0);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdiff0);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec0);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		break;
	// set material: metal
	case 1:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb1);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdiff1);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec1);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		break;
	// set material: gold
	case 2:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb2);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdiff2);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec2);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		break;

	// set material: emerald
	case 3:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb3);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdiff3);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec3);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		break;
	default:
		break;
	}
}

void Train::InitializeLocation(double beta)
{
	glTranslated(cx, cy + 0.2, cz); // position the Train in its center (we lift the Train by 0.2 to put wheels on road)
	   // only when the motion is along X axis
	beta = atan(diry); // in rad
	glRotated(beta * 180 / PI, 0, 0, 1);
	glScaled(0.2, 0.2, 0.2);
}

// compute the direction and update Train center
void Train::Move(double ground[GSZ][GSZ])
{
	int row=0, col=0;
	if (fabs(dirz) < 0.01 && fabs(dirx)>0.99) // if the direction is along X axis
	{
		// find column and row of cell in ground
		row = (int) (cz+GSZ/2);
		col = (int) (cx+GSZ/2);
		if (dirx > 0) // moving to the right
		{
			if (col >= GSZ - 1) // if the Train goes out of matrix return it back to the start
			{
				col = 0;
				cx = -GSZ / 2;
			}
		}
		else // moving to the left
		{
			if (col < 0)
			{
				col = GSZ - 2;
				cx = GSZ / 2;
				if (ground[row][col + 1] > 0) cy = ground[row][col + 1];
				else cy = 1;
			}
		}
	}
	// update center of a Train
	cx += dirx * speed;
	cz += dirz * speed;
}

void Train::SetSpeed(double s)
{
	speed = s;
}

double Train::GetCx()
{
	return cx;
}

double Train::GetCy()
{
	return cy;
}

double Train::GetCz()
{
	return cz;
}

double Train::GetDirx()
{
	return dirx;
}

double Train::GetDiry()
{
	return diry;
}

double Train::GetDirz()
{
	return dirz;
}

void Train::Draw(double offset)
{
	double beta = 0;

	glPushMatrix();
	InitializeLocation(beta);
	glPushMatrix();
	DrawTrain(offset, 1, 100, 0, 0.8, 0.8, 0.5, true); // train
	glTranslated(-18, 0, 0);
	DrawCart(offset, 3, 100, 0.6, 0.8, 0.6, 0.5, true, true); // first cart
	glTranslated(-18, 0, 0);
	DrawCart(offset, 2, 100, 1, 1, 0.4, 0.5, true, false); // second cart
	glPopMatrix();
	glPopMatrix();
}