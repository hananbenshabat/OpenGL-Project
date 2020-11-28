#pragma once
const double PI = 3.14;
const int GSZ = 150;

class Train
{
private:
	double cx, cy,cz; // center in world coordinates
	double dirx, diry, dirz;
	double speed;

	// material: red plastic
	float mamb0[4] = { 0.4,0.2,0.2,0 };
	float mdiff0[4] = { 0.9,0.0,0.0,0 };
	float mspec0[4] = { 0.9,0.7,0.7,0 };

	// material: metal
	float mamb1[4] = { 0.2,0.2,0.2,0 };
	float mdiff1[4] = { 0.156,0.156,0.3,0 };
	float mspec1[4] = { 0.156,0.156,0.3,0 };

	// material: gold
	float mamb2[4] = { 0.24,0.2,0.07,0 };
	float mdiff2[4] = { 0.75,0.6,0.22,0 };
	float mspec2[4] = { 0.62,0.55,0.37,0 };

	// material: emerald
	float mamb3[4] = { 0.2,0.4,0.2,0 };
	float mdiff3[4] = { 0.0,0.4,0.0,0 };
	float mspec3[4] = { 0.7,0.9,0.7,0 };

public:
	Train(double x, double y, double z, double dx, double dy, double dz, double speed);
	void DrawTyer(int n, double outer, double inner, int jump);
	void DrawCylinder(int n);
	void DrawConus(int n);
	void DrawLitCylinder1(int n, double topr, double bottomr);
	void DrawLitSphere(int n, int sl);
	void DrawWheel(double offset);
	void DrawWindows(bool right, bool left, bool front, bool rear, double r, double g, double b, double alpha);
	void DrawLowerBody();
	void DrawPanels();
	void DrawTrainWheels(double offset);
	void DrawTrainDriverArea();
	void DrawTrainEngineArea();
	void DrawTrain(double offset, int cartMaterial, int cartShininess,
		double windowColorR, double windowColorG, double windowColorB, double alpha, bool isRearConnected);
	void DrawTrainChimneySmoke();
	void DrawTrainUpperBody();
	void DrawTrainConnections(bool isRearConnected);
	void DrawCartWheels(double offset);
	void DrawCartUpperBody();
	void DrawCartRoof();
	void DrawCartConnections(bool isFrontConnected, bool isRearConnected);
	void DrawCartWindows(double windowColorR, double windowColorG, double windowColorB, double alpha);
	void DrawCart(double offset, int cartMaterial, int cartShininess,
		double windowColorR, double windowColorG, double windowColorB, double alpha, bool isFrontConnected, bool isRearConnected);
	void Draw(double offset);
	void SetMaterial(int materialNumber, int shininess);
	void InitializeLocation(double beta);
	void Move(double ground[GSZ][GSZ]);
	void SetSpeed(double s);
	double GetCx();
	double GetCy();
	double GetCz();
	double GetDirx();
	double GetDiry();
	double GetDirz();
};

