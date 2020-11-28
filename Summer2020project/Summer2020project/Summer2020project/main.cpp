#define _CRT_SECURE_NO_WARNINGS
#include "glut.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "Train.h"

GLuint drawTree1;
GLuint drawTree2;
GLuint drawTree3;
GLuint drawTree4;
GLuint drawTree5;
GLuint drawTree6;

const int W = 600;
const int H = 600;
const int TW = 512;
const int TH = 512;
const int NUM_TrainS = 1;
const int NUM_Trees = 6;

double ground[GSZ][GSZ] = { 0 }; // this is the matrix of heights of the ground
double treesHeights[NUM_Trees] = { 0 }; // this is the matrix of heights of the trees
double treesBases[NUM_Trees] = { 0 }; // this is the matrix of length of the bases of the trees
int treesiTree[NUM_Trees] = { 0 }; // this is the matrix of the first index of the trees
int treesjTree[NUM_Trees] = { 0 }; // this is the matrix of the second index of the trees

// texture definitions
unsigned char tx0[TH][TW][3]; // 3 stands for rgb
unsigned char tx1[1024][1024][3]; // water 
unsigned char tx2[1024][2048][3]; // sunrise 
unsigned char tx3[1024][1024][3]; // grass 
unsigned char tx4[512][512][3]; // wood 
unsigned char tx5[512][512][3]; // leaves 
unsigned char tx6[1024][1024][3]; // snow 



unsigned char* bmp;

// locomotion (ego-motion)
double eyex=0, eyey=14, eyez = 26;

double yaw=PI, pitch = -0.1, roll;
double speed = 0, angularSpeed = 0;
double dir[3] = {sin(yaw),sin(pitch),cos(yaw)};

// aircraft motion
double airyaw = PI, airpitch = 0;
double airspeed = 0, airangularSpeed = 0;
double airdir[3] = { sin(airyaw),sin(airpitch),cos(airyaw) };

double offset = 0;

Train* pTrains[NUM_TrainS] = {0};

bool buildTerrain = false;

// functions
void DrawTrainWheels();
void DrawCartWheels();
void DrawLowerBody();
void DrawTrainUpperBody();
void DrawCartUpperBody();
void DrawPanels();
void DrawWindows(bool right, bool left, bool front, bool rear, double r, double g, double b, double alpha);
void DrawCartWindows(double windowColorR, double windowColorG, double windowColorB, double alpha);
void DrawCartRoof();
void DrawCartConnections(bool isFrontConnected, bool isRearConnected);
void DrawTrainConnections(bool isRearConnected);
void DrawBridge();
void DrawWoodPlanks();
void DrawBridgeLedges();
void DrawMountain(int x, int z, int h, int sizex, int sizez);
void DrawMountains();
void DrawTree(double r, double h);
void DrawTrees();
void UpdateTerrain();
void Smooth();
void PrepareTrees();
void SetMaterial(int materialNumber, int shininess);

// first light source
float lamb0[4] = { 0.2,0.2,0.2,0 };
float ldiff0[4] = { 0.8,0.5,0.2,0 };
float lspec0[4] = { 0.5,0.5,0.5,0 };
float lpos0[4] = { -1,0.2,0,0};// the last element defines kind of light source:
// if it is 1 the the light is positional; if it is 0 the the light is directional

// second light source
float lamb1[4] = { 0.2,0.2,0.2,0 };
float ldiff1[4] = { 0.8,0.8,0.8,0 };
float lspec1[4] = { 0.7,0.7,0.7,0 };
float lpos1[4] = { 1,1,1,0 }; // directional

// material: red plastic
float mamb0[4] = { 0.4,0.2,0.2,0 };
float mdiff0[4] = { 0.9,0.0,0.0,0 };
float mspec0[4] = { 0.9,0.7,0.7,0 };

// material: green plastic
float mamb1[4] = { 0.2,0.4,0.2,0 };
float mdiff1[4] = { 0.0,0.4,0.0,0 };
float mspec1[4] = { 0.7,0.9,0.7,0 };

// material: gold
float mamb2[4] = { 0.24,0.2,0.07,0 };
float mdiff2[4] = { 0.75,0.6,0.22,0 };
float mspec2[4] = { 0.62,0.55,0.37,0 };

// material: silver
float mamb3[4] = { 0.2,0.2,0.2,0 };
float mdiff3[4] = { 0.7,0.7,0.7,0 };
float mspec3[4] = { 0.7,0.7,0.7,0 };

// material: wood
float mamb4[4] = { 0.2,0.2,0.2,0 };
float mdiff4[4] = { 0.623,0.4,0,0 };
float mspec4[4] = { 0.623,0.4,0,0 };

// material: metal
float mamb5[4] = { 0.2,0.2,0.2,0 };
float mdiff5[4] = { 0.156,0.156,0.3,0 };
float mspec5[4] = { 0.156,0.156,0.3,0 };

// material: black tyre
float mamb6[4] = { 0.2,0.2,0.2,0 };
float mdiff6[4] = { 0.2,0.2,0.2,0 };
float mspec6[4] = { 0.2,0.2,0.2,0 };

// material: gray tyre
float mamb7[4] = { 0.4,0.4,0.4,0 };
float mdiff7[4] = { 0.4,0.4,0.4,0 };
float mspec7[4] = { 0.4,0.4,0.4,0 };

// material: white tyre
float mamb8[4] = { 0.8,0.8,0.8,0 };
float mdiff8[4] = { 0.8,0.8,0.8,0 };
float mspec8[4] = { 0.8,0.8,0.8,0 };

// material: bridge ledges compoment
float mamb9[4] = { 0.6,0.6,0.6,0 };
float mdiff9[4] = { 0.7,0.7,0.7,0 };
float mspec9[4] = { 0.7,0.7,0.7,0 };

void ReadBitmap(const char fname[])
{
	FILE* pf;
	// BITMAP file starts with 
	BITMAPFILEHEADER bf; 
	// then goes
	BITMAPINFOHEADER bi;
	// and then goes stream: bgrbgrbgr.....
	int size; // how many BYTES will be read

	pf = fopen(fname, "rb");
	fread(&bf, sizeof(BITMAPFILEHEADER), 1, pf);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, pf);
	size = bi.biHeight * bi.biWidth * 3;

	bmp = (unsigned char*) malloc(size);
	fread(bmp, 1, size, pf);
	fclose(pf);
}

void SetTexture(int kind)
{
	int i, j,delta,k;
	switch (kind)
	{
	case 0: // brick wall
		for(i=0;i<TH;i++)
			for (j = 0; j < TW; j++)
			{
				delta = -20 + rand() % 40; // random value in range [-20,20)
				if (i % (TH / 2) > (TH / 2 - 15) ||
					(i<TH/2)&&(j%(TW/2)>(TW/2-15))||
					(i >= TH / 2) && (j % (TW / 4) > (TW / 4 - 15)) && j<TW/4 ||
					(i >= TH / 2) && (j % (TW / 4) > (TW / 4 - 15))&& j>3*TW/4-15 && j< 3 * TW / 4)// cement
				{
					tx0[i][j][0] = 190+delta;//red
					tx0[i][j][1] = 190 + delta;//green
					tx0[i][j][2] = 190 + delta;//blue
				}
				else				//brick
				{
					tx0[i][j][0] = 190 + delta;//red
					tx0[i][j][1] = 50 + delta;//green
					tx0[i][j][2] = 30+delta;//blue
				}
			}
		break;
	case 1: // Rail gravel
		for (i = 0; i < TH; i++)
			for (j = 0; j < TW; j++)
			{
				tx0[i][j][0] = 0;//red
				tx0[i][j][1] = 0;//green
				tx0[i][j][2] = 0;//blue
			}
		break;
	case 2: // water
		for (i = 0, k = 0; i < 1024; i++)
			for (j = 0; j < 1024; j++, k += 3)
			{
				tx1[i][j][0] = bmp[k + 2]; // red
				tx1[i][j][1] = bmp[k + 1]; // green
				tx1[i][j][2] = bmp[k]; // blue
			}
		break;
	case 3: // sunrise
		for (i = 0, k = 0; i < 1024; i++)
			for (j = 0; j < 2048; j++, k += 3)
			{
				tx2[i][j][0] = bmp[k + 2]; // red
				tx2[i][j][1] = bmp[k + 1]; // green
				tx2[i][j][2] = bmp[k]; // blue
			}
		break;
	case 4: // grass
		for (i = 0, k = 0; i < 1024; i++)
			for (j = 0; j < 1024; j++, k += 3)
			{
				tx3[i][j][0] = bmp[k + 2]; // red
				tx3[i][j][1] = bmp[k + 1]; // green
				tx3[i][j][2] = bmp[k]; // blue
			}
		break;
	case 5: // wood
		for (i = 0, k = 0; i < 512; i++)
			for (j = 0; j < 512; j++, k += 3)
			{
				tx4[i][j][0] = bmp[k + 2]; // red
				tx4[i][j][1] = bmp[k + 1]; // green
				tx4[i][j][2] = bmp[k]; // blue
			}
		break;
	case 6: // leaves
		for (i = 0, k = 0; i < 512; i++)
			for (j = 0; j < 512; j++, k += 3)
			{
				tx5[i][j][0] = bmp[k + 2]; // red
				tx5[i][j][1] = bmp[k + 1]; // green
				tx5[i][j][2] = bmp[k]; // blue
			}
		break;
	case 7: // snow
		for (i = 0, k = 0; i < 1024; i++)
			for (j = 0; j < 1024; j++, k += 3)
			{
				tx6[i][j][0] = bmp[k + 2]; // red
				tx6[i][j][1] = bmp[k + 1]; // green
				tx6[i][j][2] = bmp[k]; // blue
			}
		break;
	}
}

void PrepareGround(int startx, int startz, int endx, int endz, int height)
{
	for (int i = startz; i <= endz; i++)
		for (int j = startx; j <= endx; j++)
			ground[i][j] = height;
}


void init()
{
	srand(time(0)); // seed or init random numbers generator
	glClearColor(0.0, 0.6, 0.8, 0);// color of window background
	glEnable(GL_DEPTH_TEST); // allow to show the nearest object
	PrepareGround(0, 0, GSZ - 1, GSZ - 1, -3);// initial terrain
	
	UpdateTerrain();

	glShadeModel(GL_SMOOTH);
	drawTree1 = glGenLists(1);
	glNewList(drawTree1, GL_COMPILE);
	DrawTree(treesHeights[0], treesBases[0]);
	glEndList();

	drawTree2 = glGenLists(1);
	glNewList(drawTree2, GL_COMPILE);
	DrawTree(treesHeights[1], treesBases[1]);
	glEndList();

	drawTree3 = glGenLists(1);
	glNewList(drawTree3, GL_COMPILE);
	DrawTree(treesHeights[2], treesBases[2]);
	glEndList();

	drawTree4 = glGenLists(1);
	glNewList(drawTree4, GL_COMPILE);
	DrawTree(treesHeights[3], treesBases[3]);
	glEndList();

	drawTree5 = glGenLists(1);
	glNewList(drawTree5, GL_COMPILE);
	DrawTree(treesHeights[4], treesBases[4]);
	glEndList();

	drawTree6 = glGenLists(1);
	glNewList(drawTree6, GL_COMPILE);
	DrawTree(treesHeights[5], treesBases[5]);
	glEndList();
	Smooth();
	Smooth();

	PrepareGround(0, GSZ / 2 - 1, GSZ - 1, GSZ / 2 + 1, 1);//rail
	PrepareGround(GSZ / 2 - 1, 0, GSZ / 2 + 1, GSZ - 1, -5);//bridge
	PrepareGround(GSZ / 2 - 3, 0, GSZ / 2 - 2, GSZ - 1, -4);//left land near bridge
	PrepareGround(GSZ / 2 + 2, 0, GSZ / 2 + 3, GSZ - 1, -4);//right land near bridge

	glEnable(GL_NORMALIZE);// needs for lighting to normalize the vectors
	
	// Textures definitions
	SetTexture(0); // brick wall
	glBindTexture(GL_TEXTURE_2D, 0); // 0 stands for texture number
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TW, TH, 0, GL_RGB, GL_UNSIGNED_BYTE, tx0);

	SetTexture(1); // Rail
	glBindTexture(GL_TEXTURE_2D, 1); // 0 stands for texture number
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TW, TH, 0, GL_RGB, GL_UNSIGNED_BYTE, tx0);

	// read water from file
	ReadBitmap("water.bmp");
	SetTexture(2);
	free(bmp);
	glBindTexture(GL_TEXTURE_2D, 2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, tx1);

	// read sunrise from file
	ReadBitmap("sunrise.bmp");
	SetTexture(3);
	free(bmp);
	glBindTexture(GL_TEXTURE_2D, 3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, tx2);

	// read grass from file
	ReadBitmap("grass.bmp");
	SetTexture(4);
	free(bmp);
	glBindTexture(GL_TEXTURE_2D, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, tx3);

	// read wood from file
	ReadBitmap("wood.bmp");
	SetTexture(5);
	free(bmp);
	glBindTexture(GL_TEXTURE_2D, 5);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tx4);

	// read wood from file
	ReadBitmap("leaves.bmp");
	SetTexture(6);
	free(bmp);
	glBindTexture(GL_TEXTURE_2D, 6);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tx5);

	// read grass from file
	ReadBitmap("snow.bmp");
	SetTexture(7);
	free(bmp);
	glBindTexture(GL_TEXTURE_2D, 7);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, tx6);

	// define a Train
	pTrains[0] = new Train(-GSZ / 3.3, ground[GSZ / 2][0], 0, 1, 0, 0, 0);
	pTrains[0]->Move(ground);
	pTrains[0]->SetSpeed(0.3);

}

void DrawMountain(int x, int z, int h, int sizex, int sizez)
{
	int counter = 1;
	int height = h;
	ground[z][x] += height;

	// First Side
	for (int i = x + 1; i < (x + sizex); i++)
	{
		if (height > 0) {
			if (i >= 0 && i < 150) {
				ground[z][i] += height;
				if ((x - counter) >= 0 && (x - counter) < 150) {
					ground[z][x - counter] += height;
				}
			}
			counter++;
			height--;
		}
	}

	height = h;
	counter = 1;

	// Second Side
	for (int j = z + 1; j < (z + sizez); j++)
	{
		if (height > 0) {
			if (j >= 0 && j < 150) {
				ground[j][x] += height;
				if ((z - counter) >= 0 && (z - counter) < 150) {
					ground[z - counter][x] += height;
				}
			}
			counter++;
			height--;
		}
	}
}

void DrawMountains() {
	DrawMountain(90, 60, 20, 3, 3);
	DrawMountain(50, 50, 30, 5, 5);
	DrawMountain(45, 50, 30, 5, 5);
	DrawMountain(100, 20, 30, 4, 4);
	DrawMountain(40, 50, 40, 2, 2);
	DrawMountain(35, 50, 40, 2, 2);
	DrawMountain(30, 80, 50, 2, 2);
	DrawMountain(90, 105, 70, 5, 5);
	DrawMountain(130, 140, 120, 7, 7);
	DrawMountain(62, 20, 100, 6, 6);
	DrawMountain(52, 20, 100, 6, 6);
	DrawMountain(44, 100, 50, 5, 5);
	DrawMountain(87, 45, 30, 3, 3);
	DrawMountain(135, 90, 20, 3, 3);
	DrawMountain(100, 110, 40, 4, 3);
	DrawMountain(120, 90, 20, 3, 4);
	DrawMountain(25, 97, 30, 2, 2);
	DrawMountain(15, 97, 30, 2, 2);
	DrawMountain(10, 115, 40, 2, 2);
	DrawMountain(60, 125, 30, 2, 2);
	DrawMountain(40, 125, 30, 2, 2);
	DrawMountain(105, 50, 80, 2, 2);
	Smooth();
	Smooth();
	Smooth();
	Smooth();
	Smooth();
	Smooth();
	Smooth();
	Smooth();
	Smooth();
}

void UpdateTerrain()
{
	DrawMountains();

	double delta = 2;

	for (int i = 0; i < GSZ; i++)
		for (int j = 0; j < GSZ; j++)
		{
			if ((j < GSZ - 3) && i < (GSZ - 3)) {
				ground[i][j] += delta;
				ground[i][j] += (int)(sin(j) * sin(i) * 1.5);
				if ((i + j) % 4 == 0)
					ground[i][j] += delta;
				else
					ground[i][j] += delta / 2;
			}
		}

	PrepareTrees();
}

void Smooth()
{
	double tmp[GSZ][GSZ] ; 
	int i, j;

	// compute smoothing signal
	for (i = 1; i < GSZ - 1; i++)
		for (j = 1; j < GSZ - 1; j++)
			tmp[i][j] = (0.25*ground[i-1][j-1] + ground[i - 1][j]+ 0.25*ground[i - 1][j + 1]+
							ground[i][j - 1] + 4*ground[i][j] + ground[i][j + 1]+
							0.25 * ground[i + 1][j - 1] + ground[i + 1][j] + 0.25 * ground[i + 1][j + 1]) / 9.0;

	// copy the new signal
	for (i = 1; i < GSZ - 1; i++)
		for (j = 1; j < GSZ - 1; j++)
			ground[i][j] = tmp[i][j];

}

void SetColor(double h)
{
	if (fabs(h) < 7) // grass
	{
		glColor3d(0.3 + h / 40, 0.6 - fabs(h) / 20, 0);
	}
	else // rocks and snow
	{
		glColor3d(sqrt(h) / 5, sqrt(h) / 5, sqrt(h) / 4.0);
	}
}

void SetNormal(int i, int j)
{
	double n[3];

	n[0] = ground[i][j] - ground[i][j + 1];
	n[1] = 1;
	n[2] = ground[i][j] - ground[i + 1][j];
	glNormal3d(n[0], n[1], n[2]);

}

void DrawRail()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 1); // Rail texture
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // GL_MODULATE

	// Rail from left to right
	for (int j = 0; j < GSZ - 1; j++)
	{
		if (j < GSZ / 2 - 2 || j > GSZ / 2 + 1) // not on bridge
		{
			glBegin(GL_POLYGON);
			glTexCoord2d(0, 0);
			if (ground[GSZ / 2][j] > 0) 			glVertex3d(j - GSZ / 2, ground[GSZ / 2][j] + 0.1, -1); // point 1;
			else  glVertex3d(j - GSZ / 2, 1, -1); // bridge
			glTexCoord2d(1, 0);
			if (ground[GSZ / 2][j + 1] > 0)
			{
				glVertex3d(j - GSZ / 2 + 1, ground[GSZ / 2][j + 1] + 0.1, -1); // point 2
				glTexCoord2d(1, 2);   		glVertex3d(j - GSZ / 2 + 1, ground[GSZ / 2][j + 1] + 0.1, 1); // point 3
			}
			else
			{
				glVertex3d(j - GSZ / 2 + 1, 1, -1);
				glTexCoord2d(1, 2);   		glVertex3d(j - GSZ / 2 + 1, 1, 1); // point 3 on bridge
			}

			glTexCoord2d(0, 2);
			if (ground[GSZ / 2][j] > 0) 	glVertex3d(j - GSZ / 2, ground[GSZ / 2][j] + 0.1, 1); // point 4
			else glVertex3d(j - GSZ / 2, 1, 1); // point 4
			glEnd();
		}
	}

	// Cross Rails
	if (ground[GSZ / 2][GSZ / 2] > 0)
	{
		glBegin(GL_POLYGON);
		glTexCoord2d(0.5, 0.1);		glVertex3d(-1, ground[GSZ / 2][GSZ / 2] + 0.1, -1);
		glTexCoord2d(1, 0.1);		glVertex3d(1, ground[GSZ / 2][GSZ / 2] + 0.1, -1);
		glTexCoord2d(1, 0.9);    	glVertex3d(1, ground[GSZ / 2][GSZ / 2] + 0.1, 1);
		glTexCoord2d(0.5, 0.9);    	glVertex3d(-1, ground[GSZ / 2][GSZ / 2] + 0.1, 1);
		glEnd();
	}
	else // this is a bridge
	{
		glBegin(GL_POLYGON);
		glTexCoord2d(0.5, 0.1);		glVertex3d(-1, 1, -1);
		glTexCoord2d(1, 0.1);		glVertex3d(1, 1, -1);
		glTexCoord2d(1, 0.9);    	glVertex3d(1,1, 1);
		glTexCoord2d(0.5, 0.9);    	glVertex3d(-1, 1, 1);
		glEnd();
	}

	// 1: left side of bridge
	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);	
	if(ground[GSZ / 2][GSZ / 2 - 2]>0) glVertex3d(-2, ground[GSZ / 2][GSZ / 2-2] + 0.1, -1);
	else glVertex3d(-2, 1, -1);
	glTexCoord2d(1, 0);		
	if (ground[GSZ / 2][GSZ / 2 - 1] > 0)
	{
		glVertex3d(-1, ground[GSZ / 2][GSZ / 2 - 1] + 0.1, -1);
		glTexCoord2d(1, 7.5);    	glVertex3d(-1, ground[GSZ / 2][GSZ / 2 - 1] + 0.1, 1);
	}
	else // bridge
	{
		glVertex3d(-1, 1, -1);
		glTexCoord2d(1, 7.5);    	glVertex3d(-1, 1, 1);
	}
	glTexCoord2d(0, 7.5);  
	if (ground[GSZ / 2][GSZ / 2 - 2] > 0) glVertex3d(-2, ground[GSZ / 2][GSZ / 2-2] + 0.1, 1);
	else glVertex3d(-2, 1, 1);
	glEnd();

	//2. right side of bridge
	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	if (ground[GSZ / 2][GSZ / 2 +1 ] > 0) glVertex3d(1, ground[GSZ / 2][GSZ / 2 +1] + 0.1, -1);
	else glVertex3d(1, 1, -1);
	glTexCoord2d(1, 0);
	if (ground[GSZ / 2][GSZ / 2 + 2] > 0)
	{
		glVertex3d(2, ground[GSZ / 2][GSZ / 2 +2] + 0.1, -1);
		glTexCoord2d(1, 7.5);    	glVertex3d(2, ground[GSZ / 2][GSZ / 2 +2] + 0.1, 1);
	}
	else // bridge
	{
		glVertex3d(2, 1, -1);
		glTexCoord2d(1, 7.5);    	glVertex3d(2, 1, 1);
	}
	glTexCoord2d(0, 7.5);
	if (ground[GSZ / 2][GSZ / 2 +1] > 0) glVertex3d(1, ground[GSZ / 2][GSZ / 2 +1] + 0.1, 1);
	else glVertex3d(1, 1, 1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}


// draws the ground matrix
void DrawFloor()
{
	int i, j;
	glColor3d(0, 0, 0.3);

	// i goes on z, j goes on x, y is taken from ground[i][j]
	for (i = 0; i < GSZ - 2; i++)
		for (j = 0; j < GSZ - 2; j++)
		{
			if (fabs(ground[i][j]) >= 15) // snow
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 7); // snow texture
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // GL_REPLACE

				// set material: silver
				SetMaterial(3, 50);
			}
			else {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 4); // grass texture
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // GL_REPLACE

				// set material: green plastic
				SetMaterial(1, 50);
			}

			glBegin(GL_POLYGON);// GL_LINE_LOOP);
			SetColor(ground[i][j]);
			SetNormal(i, j);
			glTexCoord2d(0, 0);		glVertex3d(j - GSZ / 2, ground[i][j], i - GSZ / 2);
			SetColor(ground[i][j + 1]);
			SetNormal(i, j+1);
			glTexCoord2d(0, 1);		glVertex3d(j + 1 - GSZ / 2, ground[i][j + 1], i - GSZ / 2);
			SetColor(ground[i + 1][j + 1]);
			SetNormal(i+1, j+1);
			glTexCoord2d(1, 1);		glVertex3d(j + 1 - GSZ / 2, ground[i + 1][j + 1], i + 1 - GSZ / 2);
			SetColor(ground[i + 1][j]);
			SetNormal(i+1, j);
			glTexCoord2d(1, 0);		glVertex3d(j - GSZ / 2, ground[i + 1][j], i + 1 - GSZ / 2);
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}

	DrawRail();


	//river
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 2); // water texture
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // GL_REPLACE
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0, 0.3, 0.7, 0.6);
	glBegin(GL_POLYGON);// GL_LINE_LOOP);
	SetNormal(i, j);
	glTexCoord2d(0, 0);		glVertex3d(-2, -1, -GSZ / 2);
	SetNormal(i, j+1);
	glTexCoord2d(0, 1);		glVertex3d(-2, -5, GSZ / 2);
	SetNormal(i+1, j+1);
	glTexCoord2d(1, 1);		glVertex3d(2, -5, GSZ / 2);
	SetNormal(i+1, j);
	glTexCoord2d(1, 0);		glVertex3d(2, -1, -GSZ / 2);
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void DrawCylinder(int n)
{
	double alpha, teta = 2 * PI / n;

	for (alpha = 0; alpha <= 2 * PI; alpha += teta)
	{
		// defines one side

		glBegin( GL_POLYGON);
		glColor3d(fabs(sin(alpha)),(1 + cos(alpha)) / 2 ,1 - fabs(sin(alpha+PI/2)));
		glVertex3d(sin(alpha), 1, cos(alpha)); // vertex 1
		glVertex3d(sin(alpha + teta), 1, cos(alpha + teta)); // vertex 2
		glColor3d(fabs(sin(alpha))/2, (1 + cos(alpha)) / 4, 1 - fabs(sin(alpha+PI/2)));
		glVertex3d(sin(alpha + teta), 0, cos(alpha + teta )); // vertex 3
		glVertex3d(sin(alpha), 0, cos(alpha)); // vertex 4
		glEnd();
	}
}

void DrawCylinder3(int n, double r, double g,double b)
{
	double alpha, teta = 2 * PI / n;

	glColor3d(r, g, b);
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

// DrawTexCylinder2 attaches vertical and horizontal part of texture
// on cylinder
// tnum is texture number
// num_repeat is a number of texture repeats
// tpart and bpart are vertical texture coordinates
void DrawTexCylinder2(int n, int tnum, int num_repeat, double tr, double br,double tpart, double bpart)
{
	double alpha, teta = 2 * PI / n;
	double part = num_repeat / (double)n;
	int counter;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tnum);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 


	for (alpha = 0, counter = 0; alpha <= 2 * PI; alpha += teta, counter++)
	{
		// defines one side

		glBegin(GL_POLYGON);
		glTexCoord2d(counter * part, tpart);    glVertex3d(tr * sin(alpha), 1, tr * cos(alpha)); // vertex 1
		glTexCoord2d((counter + 1) * part, tpart);    		glVertex3d(tr * sin(alpha + teta), 1, tr * cos(alpha + teta)); // vertex 2
		glTexCoord2d((counter + 1) * part, bpart);    		glVertex3d(br * sin(alpha + teta), 0, br * cos(alpha + teta)); // vertex 3
		glTexCoord2d(counter * part, bpart);    		glVertex3d(br * sin(alpha), 0, br * cos(alpha)); // vertex 4
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);

}


// topr is top radius, bottomr is bottom radius
void DrawCylinder1(int n, double topr, double bottomr)
{
	double alpha, teta = 2 * PI / n;

	for (alpha = 0; alpha <= 2 * PI; alpha += teta)
	{
		// defines one side
		glBegin(GL_POLYGON);
			glColor3d(fabs(sin(alpha)), (1 + cos(alpha)) / 2, 1 - fabs(sin(alpha + PI / 2)));
		glVertex3d(topr*sin(alpha), 1, topr * cos(alpha)); // vertex 1
		glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // vertex 2
			glColor3d(fabs(sin(alpha)) / 2, (1 + cos(alpha)) / 4, 1 - fabs(sin(alpha + PI / 2)));
		glVertex3d(bottomr*sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // vertex 3
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // vertex 4
		glEnd();
	}
}

// topr is top radius, bottomr is bottom radius
// defines vector of normal
void DrawLitCylinder1(int n, double topr, double bottomr)
{
	double alpha, teta = 2 * PI / n,h;

	for (alpha = 0; alpha <= 2 * PI; alpha += teta)
	{
		h = (bottomr - topr) * bottomr;
		
		// defines one side
		glBegin(GL_POLYGON);
			glNormal3d(topr*sin(alpha), h, topr*cos(alpha));
		glVertex3d(topr * sin(alpha), 1, topr * cos(alpha)); // vertex 1
			glNormal3d(topr * sin(alpha+teta), h, topr * cos(alpha+teta));
		glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // vertex 2
			glNormal3d(bottomr * sin(alpha + teta), h, bottomr * cos(alpha + teta));
		glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // vertex 3
			glNormal3d(bottomr * sin(alpha), h, bottomr * cos(alpha));
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // vertex 4
		glEnd();
	}
}

void DrawConus(int n, double r, double g, double b)
{
	double alpha, teta = 2 * PI / n;

	for (alpha = 0; alpha <= 2 * PI; alpha += teta)
	{
		// defines one side

		glBegin(GL_POLYGON);
		glColor3d(r,g,b);
		glVertex3d(0, 1, 0); // vertex 1
		glVertex3d(sin(alpha + teta), 0, cos(alpha + teta)); // vertex 2
		glVertex3d(sin(alpha), 0, cos(alpha)); // vertex 3
		glEnd();
	}
}

// n is the number of sectors, sl is the number of slices
void DrawSphere(int n, int sl)
{
	double beta, gamma = PI / sl;


	for (beta = -PI / 2; beta <= PI / 2; beta+=gamma)
	{
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta+gamma)-sin(beta), 1);
		DrawCylinder1(n, cos(beta+gamma),cos(beta) );
		glPopMatrix();
	}
}

// n is the number of sectors, sl is the number of slices
// num_rep is amount of horizontal repeats of a texture
// vert_rep is amount of vertical repeats of a texture
void DrawTexSphere(int n, int sl,int tnum,int num_rep,int vert_rep)
{
	double beta, gamma = PI / sl;
	int counter;
	double part= vert_rep/(double)sl;

	for (beta = -PI / 2,counter=0; beta <= PI / 2; beta += gamma,counter++)
	{
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta + gamma) - sin(beta), 1);
		DrawTexCylinder2(n, tnum,num_rep,cos(beta + gamma), cos(beta),(counter+1)*part,counter*part);
		glPopMatrix();
	}
}

// n is the number of sectors, sl is the number of slices
void DrawLitSphere(int n, int sl)
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

void DrawTree(double h, double r) {

	float angle;
	glPushMatrix();
	glScaled(1, h, 1);
	DrawTexCylinder2(20, 10, 5, r, r - (0.2 * r), r, r - (0.2 * r));
	glPopMatrix();
	glTranslated(0, h, 0);
	h -= h * .2; r -= r * 0.3;
	for (int branches = 0; branches < 3; branches++) {
		angle = rand() % 50 + 20;
		if (angle > 48)
			angle = -(rand() % 50 + 20);

		if (h > 1) {
			
			// draw leaves on top of the smaller branches of the tree
			if (h < 1.5) {
				glPushMatrix();
				for (int leavesAngle = 60; leavesAngle <= 180; leavesAngle += 60) {
					glColor3d(0.7, 0.8, 0.5);
					glRotated(leavesAngle, 1, 0, 1);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, 6); // leaves
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // GL_REPLACE

					glBegin(GL_POLYGON);
					glTexCoord2d(0, 0);		glVertex3d(-2, 0, -2);
					glTexCoord2d(0, 1);		glVertex3d(-2, 0, 2);
					glTexCoord2d(1, 1);		glVertex3d(2, 0, 2);
					glTexCoord2d(1, 0);		glVertex3d(2, 0, -2);
					glEnd();
					glDisable(GL_TEXTURE_2D);
				}
				glPopMatrix();
			}
			
			glPushMatrix();
			glRotated(angle, 1, 0, 1);
			DrawTree(h, r);
			glPopMatrix();

		}
	}
}

void PrepareTrees() {
	int treeCounter = 0;
	double treeBase = rand() % 4;
	double treeHeight = rand() % 15;

	// Tree 1:
	treesiTree[treeCounter] = 15;
	treesjTree[treeCounter] = 30;
	treeHeight = treeHeight * 0.1 + 2;
	treesHeights[treeCounter] = treeHeight;
	treeBase = (treeBase + 1) * 0.1;
	treesBases[treeCounter] = treeBase;
	treeCounter++;

	// Tree 2:
	treesiTree[treeCounter] = 40;
	treesjTree[treeCounter] = 23;
	treeHeight = rand() % 15;
	treeHeight = treeHeight * 0.1 + 2;
	treesHeights[treeCounter] = treeHeight;
	treeBase = rand() % 4;
	treeBase = (treeBase + 1) * 0.1;
	treesBases[treeCounter] = treeBase;
	treeCounter++;

	// Tree 3:
	treesiTree[treeCounter] = 60;
	treesjTree[treeCounter] = 50;
	treeHeight = rand() % 15;
	treeHeight = treeHeight * 0.1 + 2;
	treesHeights[treeCounter] = treeHeight;
	treeBase = rand() % 4;
	treeBase = (treeBase + 1) * 0.1;
	treesBases[treeCounter] = treeBase;
	treeCounter++;

	// Tree 4:
	treesiTree[treeCounter] = 120;
	treesjTree[treeCounter] = 110;
	treeHeight = rand() % 15;
	treeHeight = treeHeight * 0.1 + 2;
	treesHeights[treeCounter] = treeHeight;
	treeBase = rand() % 4;
	treeBase = (treeBase + 1) * 0.1;
	treesBases[treeCounter] = treeBase;
	treeCounter++;

	// Tree 5:
	treesiTree[treeCounter] = 20;
	treesjTree[treeCounter] = 10;
	treeHeight = rand() % 15;
	treeHeight = treeHeight * 0.1 + 2;
	treesHeights[treeCounter] = treeHeight;
	treeBase = rand() % 4;
	treeBase = (treeBase + 1) * 0.1;
	treesBases[treeCounter] = treeBase;
	treeCounter++;


	// Tree 6:
	treesiTree[treeCounter] = 35;
	treesjTree[treeCounter] = 48;
	treeHeight = rand() % 15;
	treeHeight = treeHeight * 0.1 + 2;
	treesHeights[treeCounter] = treeHeight;
	treeBase = rand() % 4;
	treeBase = (treeBase + 1) * 0.1;
	treesBases[treeCounter] = treeBase;
	treeCounter++;
}

void DrawTyer(int n,double outer, double inner,double r, double g, double b,int jump)
{
	// kind of ring
	double alpha, teta = 2*PI/n;
	double x, z;
	glColor3d(r, g, b);
	for (alpha = 0; alpha < 2 * PI; alpha += jump*teta)
	{
		glBegin(GL_POLYGON);
		x = outer * sin(alpha);
		z = outer * cos(alpha);
		glVertex3d(x, 0, z); // 1
		x = inner * sin(alpha);
		z = inner * cos(alpha);
		glVertex3d(x, 0, z); // 2
		x = inner * sin(alpha+teta);
		z = inner * cos(alpha + teta);
		glVertex3d(x, 0, z); // 3
		x = outer * sin(alpha + teta);
		z = outer * cos(alpha + teta);
		glVertex3d(x, 0, z); // 4
		glEnd();
	}
}

void DrawWheel()
{
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glRotated(-50*offset, 0, 1, 0);

	// set material: black tyre
	SetMaterial(6, 100);

	DrawCylinder3(50, 0.2, 0.2, 0.2);
	glPushMatrix();
	glScaled(0.7, 1, 0.7);
	DrawCylinder3(50, 0.2, 0.2, 0.2);
	glPopMatrix();

	// set material: gray tyre
	SetMaterial(7, 100);

	DrawTyer(50, 1, 0.7, 0.4, 0.4, 0.4, 1);

	// set material: white tyre
	SetMaterial(8, 100);;

	DrawTyer(20, 0.7, 0, 0.8, 0.8, 0.8, 2);
	glPushMatrix();
	glTranslated(0, 1, 0);

	// set material: gray tyre
	SetMaterial(7, 100);

	DrawTyer(50, 1, 0.7, 0.4, 0.4, 0.4,1);

	// set material: white tyre
	SetMaterial(8, 100);


	DrawTyer(20, 0.7, 0, 0.8, 0.8, 0.8, 2);

	glPopMatrix();
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

void DrawTrees()
{
	// Tree 1:
	glPushMatrix();
	glTranslated(-treesiTree[0], ground[treesiTree[0]][treesjTree[0]], -treesjTree[0]);
	glCallList(drawTree1);
	glPopMatrix();

	// Tree 2:
	glPushMatrix();
	glTranslated(-treesiTree[1], ground[treesiTree[1]][treesjTree[1]], treesjTree[1]);
	glCallList(drawTree2);
	glPopMatrix();

	// Tree 3:
	glPushMatrix();
	glTranslated(treesiTree[2], ground[treesiTree[2]][treesjTree[2]], -treesjTree[2]);
	glCallList(drawTree3);
	glPopMatrix();

	// Tree 4:
	glPushMatrix();
	glTranslated(treesiTree[3], ground[treesiTree[3]][treesjTree[3]], treesjTree[3]);
	glCallList(drawTree4);
	glPopMatrix();

	// Tree 5:
	glPushMatrix();
	glTranslated(-treesiTree[4], ground[treesiTree[4]][treesjTree[4]], -treesjTree[4]);
	glCallList(drawTree5);
	glPopMatrix();

	// Tree 6:
	glPushMatrix();
	glTranslated(treesiTree[5], ground[treesiTree[5]][treesjTree[5]], treesjTree[5]);
	glCallList(drawTree6);
	glPopMatrix();
}

void DrawTrain(int trainMaterial, int trainShininess, 
	double windowColorR, double windowColorG, double windowColorB, double alpha, bool isRearConnected)
{
	DrawTrainWheels();
	SetMaterial(trainMaterial, trainShininess);
	DrawLowerBody();
	DrawTrainUpperBody();
	DrawTrainConnections(isRearConnected);
	DrawWindows(true, true, true, true, windowColorR, windowColorG, windowColorB, alpha);
}

void DrawCart(int cartMaterial, int cartShininess,
	double windowColorR, double windowColorG, double windowColorB, double alpha, bool isFrontConnected, bool isRearConnected)
{
	DrawCartWheels();
	SetMaterial(cartMaterial, cartShininess);
	DrawLowerBody();
	DrawCartUpperBody();
	DrawCartRoof();
	DrawPanels();
	DrawCartConnections(isFrontConnected, isRearConnected);
	DrawCartWindows(windowColorR, windowColorG, windowColorB, alpha);
}

void DrawTrainWheels()
{
	glEnable(GL_LIGHTING);

	glPushMatrix();
	glTranslated(6, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(6, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-6, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-6, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(3.5, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(3.5, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-3.5, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-3.5, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.25, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.25, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-1.25, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-1.25, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();
}

void DrawTrainDriverArea()
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

void DrawTrainEngineArea()
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
	DrawConus(50, 1, 1, 1);
	glPopMatrix();

	// front lighting
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
	SetMaterial(5, 100);

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

void DrawTrainChimneySmoke()
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

void DrawTrainUpperBody()
{
	DrawTrainDriverArea();
	DrawTrainEngineArea();
	DrawTrainChimneySmoke();
}

void DrawTrainConnections(bool isRearConnected)
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

void DrawCartWheels()
{
	glEnable(GL_LIGHTING);
	
	glPushMatrix();
	glTranslated(6, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(6, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-6, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-6, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(2, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(2, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-2, 0, 2.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-2, 0, -3.5);
	glRotated(90, 1, 0, 0);
	DrawWheel();
	glPopMatrix();
}

void DrawLowerBody()
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

void DrawCartUpperBody()
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

void DrawCartRoof()
{
	glPushMatrix();
	glTranslated(-0.1, 7.1, 0);
	glScaled(2.3, 0.03, 0.9);
	glutSolidCube(6);
	glPopMatrix();
}

void DrawPanels()
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

void DrawCartConnections(bool isFrontConnected, bool isRearConnected)
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

void DrawCartWindows(double windowColorR, double windowColorG, double windowColorB, double alpha)
{
	glPushMatrix();
	DrawWindows(true, true, true, false , windowColorR, windowColorG, windowColorB, alpha);
	glTranslated(3.81, 0, 0);
	DrawWindows(true, true, false, false, windowColorR, windowColorG, windowColorB, alpha);
	glTranslated(3.31, 0, 0);
	DrawWindows(true, true, false, false, windowColorR, windowColorG, windowColorB, alpha);
	glTranslated(3.32, 0, 0);
	DrawWindows(true, true, false, true, windowColorR, windowColorG, windowColorB, alpha);
	glPopMatrix();
}

void DrawWindows(bool right, bool left, bool front, bool rear, double r, double g, double b, double alpha)
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

void DrawBridge()
{
	// set material: silver
	SetMaterial(3, 50);

	// 4 main support pillars:
	glPushMatrix();
	glTranslated(-3, 1, -2);
	glScaled(0.3, 7, 0.3);
	glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-3, 8, -2);
	glScaled(1, 1, 1);
	glutSolidSphere(0.6, 30, 30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-3, 1, 2);
	glScaled(0.3, 7, 0.3);
	glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-3, 8, 2);
	glScaled(1, 1, 1);
	glutSolidSphere(0.6, 30, 30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(3, 1, -2);
	glScaled(0.3, 7, 0.3);
	glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
	glTranslated(3, 8, -2);
	glScaled(1, 1, 1);
	glutSolidSphere(0.6, 30, 30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(3, 1, 2);
	glScaled(0.3, 7, 0.3);
	glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
	glTranslated(3, 8, 2);
	glScaled(1, 1, 1);
	glutSolidSphere(0.6, 30, 30);
	glPopMatrix();

	// bridge
	glPushMatrix();
	glTranslated(0, 0.45, 0);
	glScaled(-3, 0.5, -2.5);
	glutSolidCube(2);
	glPopMatrix();

	DrawBridgeLedges();
}

void DrawBridgeLedges()
{
	// set material: bridge ledges compoment
	SetMaterial(9, 80);

	// horizontal ledges
	glPushMatrix();
	glTranslated(2.7, 1.7, -2);
	glScaled(5.5, 0.15, 0.15);
	glRotated(90, 0, 0, 1);
	DrawCylinder(30);
	glTranslated(4, 0, 0);
	DrawCylinder(30);
	glTranslated(0, 0, 26);
	DrawCylinder(30);
	glTranslated(-4, 0, 0);
	DrawCylinder(30);
	glPopMatrix();

	// vertical ledges
	glPushMatrix();
	glTranslated(1.5, 0, -2);
	glScaled(0.15, 2.6, 0.15);
	DrawCylinder(30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(1.5, 2.6, -2);
	glScaled(0.15, 0.15, 0.15);
	DrawSphere(30, 30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, -2);
	glScaled(0.15, 2.6, 0.15);
	DrawCylinder(30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 2.6, -2);
	glScaled(0.15, 0.15, 0.15);
	DrawSphere(30, 30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-1.5, 0, -2);
	glScaled(0.15, 2.6, 0.15);
	DrawCylinder(30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-1.5, 2.6, -2);
	glScaled(0.15, 0.15, 0.15);
	DrawSphere(30, 30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(1.5, 0, 2);
	glScaled(0.15, 2.6, 0.15);
	DrawCylinder(30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(1.5, 2.6, 2);
	glScaled(0.15, 0.15, 0.15);
	DrawSphere(30, 30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 2);
	glScaled(0.15, 2.6, 0.15);
	DrawCylinder(30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 2.6, 2);
	glScaled(0.15, 0.15, 0.15);
	DrawSphere(30, 30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-1.5, 0, 2);
	glScaled(0.15, 2.6, 0.15);
	DrawCylinder(30);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-1.5, 2.6, 2);
	glScaled(0.15, 0.15, 0.15);
	DrawSphere(30, 30);
	glPopMatrix();
}

void DrawWoodPlanks() 
{
	// set material: wood
	SetMaterial(4, 50);

	// Rail
	glPushMatrix();
	glTranslated(85, 0, 0);
	for (int i = 0; i < 37; i++) {
		glTranslated(-4, 0, 0);
		glPushMatrix();
		glTranslated(-9, 1.1, 0.2);
		glScaled(1, 0.02, -0.05);
		glutSolidCube(4);
		glPushMatrix();
		glTranslated(1.5, 0, 4);
		glScaled(0.1, 1.5, 5);
		glutSolidCube(4);
		glTranslated(-9.5, 0, 0);
		glutSolidCube(4);
		glTranslated(-9.5, 0, 0);
		glutSolidCube(4);
		glTranslated(-9.5, 0, 0);
		glutSolidCube(4);
		glPopMatrix();
		glTranslated(0, 0, 7);
		glutSolidCube(4);
		glPopMatrix();
	}
	glPopMatrix();

}

void DrawSky()
{
	// textured sphere
	glPushMatrix();
	glTranslated(0, -30, 0);
	glRotated(offset / 10, 0, 1, 0);
	glScaled(180, 180, 180);
	DrawTexSphere(140, 40, 3, 1, 1);
	glPopMatrix();
}

void SetMaterial(int materialNumber, int shininess)
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
		// set material: green plastic
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
		// set material: silver
	case 3:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb3);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdiff3);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec3);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		break;
		// set material: wood
	case 4:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb4);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdiff4);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec4);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		break;
		// set material: metal
	case 5:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb5);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdiff5);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec5);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		break;
		// set material: black tyre
	case 6:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb6);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdiff6);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec6);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		break;
		// set material: gray tyre
	case 7:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb7);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdiff7);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec7);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		break;
		// set material: white tyre
	case 8:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb8);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdiff8);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec8);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		break;
		// set material: bridge ledges compoment
	case 9:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb9);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdiff9);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspec9);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		break;
	default:
		break;
	}
}

void EnableLights()
{
	glEnable(GL_LIGHT0); // first source of light
	glEnable(GL_LIGHT1); // second source of light

	// light definitions
	// first light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lamb0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ldiff0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lspec0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos0);

	// second light
	glLightfv(GL_LIGHT1, GL_AMBIENT, lamb1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, ldiff1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lspec1);
	glLightfv(GL_LIGHT1, GL_POSITION, lpos1);
}

void displaySide()
{
	glEnable(GL_LIGHTING);
	EnableLights();
	DrawFloor();

	// define texture of mirror
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and depth buffer
	glViewport(0, 0, W, H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // loads the Identity matrix to the Projection Matrix
	glFrustum(-0.2, 0.2, -0.2, 0.2, 0.3, 300); // define the camera model

	// define the viewing parameters
	gluLookAt(pTrains[0]->GetCx() - 5, pTrains[0]->GetCy() + 8, pTrains[0]->GetCz() + 13, // eye coordinates
		pTrains[0]->GetCx() + pTrains[0]->GetDirx(), pTrains[0]->GetCy() + 1.1 + pTrains[0]->GetDiry(),
		pTrains[0]->GetCz() + pTrains[0]->GetDirz(), // point of interest coordinates
		0, 1, 0); // vector UP reflects roll
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // loads the Identity matrix to the TRASFORMATION Matrix
	DrawFloor();
	DrawBridge();
	DrawWoodPlanks();
	DrawTrees();
	DrawSky();

	for (int i = 0; i < NUM_TrainS; i++)
		pTrains[i]->Draw(offset);
	glDisable(GL_LIGHTING);
	glutSwapBuffers(); // show all
}

void displayTrain()
{
	glEnable(GL_LIGHTING);
	EnableLights();
	DrawFloor();

	// define texture of mirror
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and depth buffer
	glViewport(0, 0, W, H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // loads the Identity matrix to the Projection Matrix
	glFrustum(-0.2, 0.2, -0.2, 0.2, 0.3, 300); // define the camera model

	// define the viewing parameters
	gluLookAt(pTrains[0]->GetCx() - 1.5, pTrains[0]->GetCy() + 1.3, pTrains[0]->GetCz(), // eye coordinates
		pTrains[0]->GetCx() + pTrains[0]->GetDirx(), pTrains[0]->GetCy() + 1.1 + pTrains[0]->GetDiry(),
		pTrains[0]->GetCz() + pTrains[0]->GetDirz(), // point of interest coordinates
		0, 1, 0); // vector UP reflects roll

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // loads the Identity matrix to the TRASFORMATION Matrix
	DrawFloor();
	DrawBridge();
	DrawWoodPlanks();
	DrawTrees();
	DrawSky();

	for (int i = 0; i < NUM_TrainS; i++)
		pTrains[i]->Draw(offset);
	glDisable(GL_LIGHTING);
	glutSwapBuffers(); // show all
}

void displayCartWindow()
{
	glEnable(GL_LIGHTING);
	EnableLights();
	DrawFloor();

	// define texture of mirror
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and depth buffer
	glViewport(0, 0, W, H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // loads the Identity matrix to the Projection Matrix
	glFrustum(-0.2, 0.2, -0.2, 0.2, 0.3, 300); // define the camera model

	// define the viewing parameters
	gluLookAt(pTrains[0]->GetCx() - 3.5, pTrains[0]->GetCy() + 1.3, pTrains[0]->GetCz(), // eye coordinates
		pTrains[0]->GetCx() + pTrains[0]->GetDirx(), pTrains[0]->GetCy() + 2.2 + pTrains[0]->GetDiry(),
		pTrains[0]->GetCz() + pTrains[0]->GetDirz() + 10, // point of interest coordinates
		0, 1, 0); // vector UP reflects roll
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // loads the Identity matrix to the TRASFORMATION Matrix
	DrawFloor();
	DrawBridge();
	DrawWoodPlanks();
	DrawTrees();
	DrawSky();

	for (int i = 0; i < NUM_TrainS; i++)
		pTrains[i]->Draw(offset);
	glDisable(GL_LIGHTING);
	glutSwapBuffers(); // show all
}

void display()
{
	// frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and depth buffer
	glViewport(0, 0, W, H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // loads the Identity matrix to the Projection Matrix
	glFrustum(-1, 1, -1, 1, 1, 300); // define the camera model

	// define the viewing parameters
	gluLookAt(eyex, eyey, eyez, // eye coordinates
		eyex + dir[0], eyey + dir[1], eyez + dir[2], // point of interest coordinates
		0, 1, 0); // vector UP reflects roll
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // loads the Identity matrix to the TRASFORMATION Matrix
	glEnable(GL_LIGHTING);
	EnableLights();
	DrawFloor();
	DrawBridge();
	DrawWoodPlanks();
	DrawTrees();
	DrawSky();

	for (int i = 0; i < NUM_TrainS; i++)
		pTrains[i]->Draw(offset);
	glDisable(GL_LIGHTING);
	glutSwapBuffers(); // show all
}

void idle()
{
	offset += 0.2;
	
	// set locomotion direction 
	yaw += angularSpeed;

	// setup the sight direction
	dir[0] = sin(yaw);
	dir[1] = sin(pitch);
	dir[2] = cos(yaw);

	// setup the motion
	eyex += speed * dir[0];
	eyey += speed * dir[1];
	eyez += speed * dir[2];
	
	for(int i=0;i<NUM_TrainS;i++)
		pTrains[i]->Move(ground);

	glutPostRedisplay(); // posts message (with request to show the frame ) to main window
}

void special_key(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		angularSpeed += 0.005;
		break;
	case GLUT_KEY_RIGHT:
		angularSpeed -= 0.005;
		break;
	case GLUT_KEY_UP: 
		speed += 0.05;
		break;
	case GLUT_KEY_DOWN:
		speed -= 0.05;
		break;
	case GLUT_KEY_PAGE_UP:
		pitch+= 0.1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		pitch -= 0.1;
		break;
	}
}

// mouse drag function
void motion(int x, int y)
{
	double xp, yp;

	xp = 2*(x / 100.0) -1 ;
	yp = 2 * (H - y) / 100.0 - 1;

	if (-0.2 < xp && xp < 0.2 && airpitch - 0.2 < yp && yp < airpitch + 0.2) // update airpitch
		airpitch = yp;
}

void menu(int choice)
{
	switch (choice)
	{
	case 1:
		glutDisplayFunc(displaySide);
		break;
	case 2:
		glutDisplayFunc(displayTrain);
		break;
	case 3:
		glutDisplayFunc(displayCartWindow);
		break;
	case 4:
		glutDisplayFunc(display);
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	// defines BUFFERS: Color buffer (frame buffer) and Depth buffer
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
	glutInitWindowSize(W, H);
	glutInitWindowPosition(300, 100);
	glutCreateWindow("Summer2020project");
	glutDisplayFunc(displaySide); // refresh window function
	glutIdleFunc(idle); // kind of timer function
	glutSpecialFunc(special_key);
	glutMotionFunc(motion);

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Side view", 1);
	glutAddMenuEntry("Train view", 2);
	glutAddMenuEntry("Cart view", 3);
	glutAddMenuEntry("Spectator view(arrow keys)", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	init();
	glutMainLoop();
}