//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fÃ¼ggvÃ©nyt kivÃ©ve, a lefoglalt adat korrekt felszabadÃ­tÃ¡sa nÃ©lkÃ¼l
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : TOTH GELLERT
// Neptun : QGZ6DV
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
	float r, g, b;

	Color() {
		r = g = b = 0;
	}
	Color(float r0, float g0, float b0) {
		r = r0;
		g = g0;
		b = b0;
	}
	Color operator*(float a) {
		return Color(r * a, g * a, b * a);
	}
	Color operator*(const Color& c) {
		return Color(r * c.r, g * c.g, b * c.b);
	}
	Color operator+(const Color& c) {
		return Color(r + c.r, g + c.g, b + c.b);
	}
};

template<class T, int size = 10>
class Array {
	T array[size];
	int numOfElements;

public:

	Array() :
			numOfElements(0) {
	}

	int SizeOf() {
		return numOfElements;
	}

	void Add(T &t) {
		if (numOfElements >= size) {
			exit(-1);
		}
		if (numOfElements == 0) {
			array[numOfElements++] = t;
			array[numOfElements] = t;
		} else {
			array[numOfElements++] = t;
			array[numOfElements] = array[0];

		}

	}

	T& operator[](int i) {
		if (i >= size)
			exit(-1);
		return array[i];
	}

};

void getMinor(float** source, float** destination, int row, int col,
		int order) {
	int colCount = 0;
	int rowCount = 0;

	for (int i = 0; i < order; i++) {
		if (i != row) {
			colCount = 0;
			for (int j = 0; j < order; j++) {
				if (j != col) {
					destination[rowCount][colCount] = source[i][j];
					colCount++;
				}
			}
			rowCount++;
		}

	}
}

float calcDeterminant(float** matrix, int order) {
	if (order == 1) {
		return matrix[0][0];
	}

	float det = 0;

	float** minor = new float*[order - 1];
	for (int i = 0; i < order - 1; i++) {
		minor[i] = new float[order - 1];
	}

	for (int i = 0; i < order; i++) {
		getMinor(matrix, minor, i, 0, order);

		det += (i % 2 == 1 ? -1 : 1) * matrix[i][0]
				* calcDeterminant(minor, order - 1);
	}

	for (int i = 0; i < order - 1; i++)
		delete[] minor[i];
	delete[] minor;

	return det;
}

struct myMatrix {
//	A B C D
//	E F G H
//	I J K L
//	M N O P

	float M[4][4];

	myMatrix() {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				M[i][j] = 0;
			}
		}
	}

	myMatrix(float a, float b, float c, float d, float e, float f, float g,
			float h, float i, float j) {
		M[0][0] = a;
		M[0][1] = b;
		M[0][2] = c;
		M[0][3] = d;
		M[1][0] = b;
		M[1][1] = e;
		M[1][2] = f;
		M[1][3] = g;
		M[2][0] = c;
		M[2][1] = f;
		M[2][2] = h;
		M[2][3] = i;
		M[3][0] = d;
		M[3][1] = g;
		M[3][2] = i;
		M[3][3] = j;
	}

	myMatrix(float a, float b, float c, float d, float e, float f, float g,
			float h, float i, float j, float k, float l, float m, float n,
			float o, float p) {
		M[0][0] = a;
		M[0][1] = b;
		M[0][2] = c;
		M[0][3] = d;
		M[1][0] = e;
		M[1][1] = f;
		M[1][2] = g;
		M[1][3] = h;
		M[2][0] = i;
		M[2][1] = j;
		M[2][2] = k;
		M[2][3] = l;
		M[3][0] = m;
		M[3][1] = n;
		M[3][2] = o;
		M[3][3] = p;
	}

	myMatrix operator+(const myMatrix & A) {
		myMatrix B;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				B.M[i][j] = M[i][j] + A.M[i][j];
			}
		}

		return B;
	}

	myMatrix operator*(float f) {
		myMatrix B;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				B.M[i][j] = M[i][j] * f;
			}
		}

		return B;
	}

	myMatrix operator*(const myMatrix & A) {
		myMatrix B;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				B.M[i][j] = 0;
				for (int k = 0; k < 4; k++) {
					B.M[i][j] = B.M[i][j] + M[i][k] * A.M[k][j];
				}

			}
		}

		return B;

	}

	void printM() {
		for (int i = 0; i < 4; i++) {
			{
				cout << M[i][0] << " " << M[i][1] << " " << M[i][2] << " "
						<< M[i][3] << endl;
			}

		}

	}

	myMatrix Transp() {

		myMatrix A;
		A.M[0][0] = M[0][0];
		A.M[0][1] = M[1][0];
		A.M[0][2] = M[2][0];
		A.M[0][3] = M[3][0];
		A.M[1][0] = M[0][1];
		A.M[1][1] = M[1][1];
		A.M[1][2] = M[2][1];
		A.M[1][3] = M[3][1];
		A.M[2][0] = M[0][2];
		A.M[2][1] = M[1][2];
		A.M[2][2] = M[2][2];
		A.M[2][3] = M[3][2];
		A.M[3][0] = M[0][3];
		A.M[3][1] = M[1][3];
		A.M[3][2] = M[2][3];
		A.M[3][3] = M[3][3];

		return A;
	}

	myMatrix inverse() {
		float** matrix = new float*[4];
		for (int i = 0; i < 4; i++)
			matrix[i] = new float[4];
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				matrix[i][j] = M[i][j];
			}
		}

		myMatrix inverse = myMatrix();

		float** minor = new float*[3];
		for (int i = 0; i < 3; i++)
			minor[i] = new float[3];

		float determinant = calcDeterminant(matrix, 4);
		if (determinant == 0)
			return inverse;

		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 4; i++) {
				getMinor(matrix, minor, j, i, 4);
				inverse.M[i][j] = calcDeterminant(minor, 3) / determinant;
				if ((i + j) % 2 == 1) {
					inverse.M[i][j] *= (-1);

				}
			}
		}

		delete[] matrix;
		delete[] minor;
		return inverse;
	}

};

struct Vector {
	float x, y, z, w;

	Vector() {
		x = y = z = w = 0;
	}
	Vector(float x0, float y0, float z0 = 0, float w0 = 1) {
		x = x0;
		y = y0;
		z = z0;
		w = w0;
	}
	Vector operator*(float a) {
		return Vector(x * a, y * a, z * a);
	}
	Vector operator*(float a) const {
		return Vector(x * a, y * a, z * a);
	}
	Vector operator/(float a) {
		return Vector(x / a, y / a, z / a);
	}
	Vector operator+(const Vector& v) {
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator+(const Vector& v) const {
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v) {
		return Vector(x - v.x, y - v.y, z - v.z, 0);
	}
	Vector operator-(const Vector& v) const {
		return Vector(x - v.x, y - v.y, z - v.z, 0);
	}
	float operator*(const Vector& v) { 	// dot product
		return (x * v.x + y * v.y + z * v.z + w * v.w);
	}
	float operator*(const Vector& v) const { 	// dot product
		return (x * v.x + y * v.y + z * v.z);
	}

	Vector operator%(const Vector& v) { 	// cross product
		return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x,
				0);
	}
	float Length() {
		return sqrt(x * x + y * y + z * z);
	}
	Vector operator+(float a) {
		return Vector(x + a, y + a, z + a);
	}

	void Normalize() {
		float l = Length();
		if (l < 0.000001f) {
			x = 0;
			y = 0;
			z = 0;
		} else {
			x /= l;
			y /= l;
			z /= l;
			w = 0;
		}
	}

	void homogen() {
		if (w != 0) {
			x /= fabs(w);
			y /= fabs(w);
			z /= fabs(w);
			w = 0;

		}
	}

	Vector operator-(void) {
		return Vector(-x, -y, -z);
	}
	void operator+=(const Vector& v) {
		x += v.x;
		y += v.y;
		z += v.z;
	}
	void operator-=(const Vector& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}
	void operator*=(float f) {
		x *= f;
		y *= f;
		z *= f;
	}

	void printOut() {
		cout << "x:" << x << " y:" << y << " z:" << z << " w:" << w << endl;
	}

	Vector operator*(const myMatrix & A) {
		float V_a[4] = { 0, 0, 0, 0 };
		float V_b[4] = { x, y, z, w };

		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 4; i++)
				V_a[j] += ((V_b[i] * (A.M[i][j])));
		}

		Vector V = Vector(V_a[0], V_a[1], V_a[2], V_a[3]);

		return V;
	}

	Vector operator*(const myMatrix & A) const {
		float V_a[4] = { 0, 0, 0, 0 };
		float V_b[4] = { x, y, z, w };

		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 4; i++)
				V_a[j] += ((V_b[i] * (A.M[i][j])));
		}

		Vector V = Vector(V_a[0], V_a[1], V_a[2], V_a[3]);

		return V;
	}

};

struct ControllPointWithTime {
	Vector controllPoint;
	long time;

//	ControllPointWithTime& operator=(const ControllPointWithTime& c) {
//		if (this != &c) {
//			controllPoint.x = c.controllPoint.x;
//			controllPoint.y = c.controllPoint.y;
//			time = c.time;
//		}
//		return *this;
//	}

};

const int screenWidth = 600;	// alkalmazÃ¡s ablak felbontÃ¡sa
const int screenHeight = 600;
const float PI_MAT = 3.14159;

Color image[screenWidth * screenHeight];	// egy alkalmazÃ¡s ablaknyi kÃ©p
Color image2[screenWidth * screenHeight];
int piecesOfControllPoints = 0;
Array<ControllPointWithTime, 10> pointsWithTime;
class Circle {
	Vector center;
	float R;

public:

	Circle() {
		center = Vector();
		R = 0;
	}
	Circle(Vector v, float r) {
		center = v;
		R = r;
	}

	void drawOut() {
		glColor3f(1, 0, 0);

		glBegin(GL_TRIANGLE_FAN);

		for (double fi = 0; fi <= 360; ++fi) {
			double fi_rad = (PI_MAT / 180) * fi;
			double x2 = cos(fi_rad) * R;
			double y2 = sin(fi_rad) * R;
			glVertex2f(center.x + x2, center.y + y2);
			glColor3f(1, 0, 0);
		}
		glEnd();

		glColor3f(1, 1, 1);
		glBegin(GL_LINE_STRIP);

		for (double fi = 0; fi <= 360; ++fi) {
			double fi_rad = (PI_MAT / 180) * fi;
			double x2 = cos(fi_rad) * R;
			double y2 = sin(fi_rad) * R;

			glVertex2f(center.x + x2, center.y + y2);
		}
		glEnd();

	}

};

bool space = false;

class Animation {

private:
	long animationTime;
	long accTime;
	Vector myDir;
	float velo;
public:
	Vector viewPortPosition;

	Animation() {

		animationTime = 0;
		viewPortPosition.x = 250;
		viewPortPosition.y = 250;
		myDir = Vector(2, 3);
		myDir.Normalize();
		accTime = 0;
		velo = 5;

	}

	Vector getNormalOfTheWall() {
		if (viewPortPosition.x <= 0) {
			return Vector(1, 0);
		} else if (viewPortPosition.x >= 500) {
			return Vector(-1, 0);
		} else if (viewPortPosition.y <= 0) {
			return Vector(0, 1);
		} else if (viewPortPosition.y >= 500) {
			return Vector(0, -1);
		} else {
			return Vector();
		}
	}

	Vector reflect() {
		Vector inDir = myDir;
		inDir.Normalize();
		Vector normVect = getNormalOfTheWall();
		normVect.Normalize();

		return inDir - normVect * (normVect * inDir) * 2;
	}

	void animation() {
		long time = glutGet(GLUT_ELAPSED_TIME);

		if (time - accTime > 500 && space && velo <= 14) {
			velo += 0.25;
			accTime = glutGet(GLUT_ELAPSED_TIME);
		}
		long ellapsedTime = time - animationTime;
		if (ellapsedTime > 50 && space) {

			animationTime = glutGet(GLUT_ELAPSED_TIME);

			myDir = reflect();

			viewPortPosition.x = myDir.x * velo + viewPortPosition.x;
			viewPortPosition.y = myDir.y * velo + viewPortPosition.y;
			glLoadIdentity();
			gluOrtho2D(viewPortPosition.x, viewPortPosition.x + 500,
					viewPortPosition.y, viewPortPosition.y + 500);

		}
	}

	void resizeViewPort() {
		animationTime = glutGet(GLUT_ELAPSED_TIME);
		accTime = animationTime;
		glLoadIdentity();
		gluOrtho2D(viewPortPosition.x, viewPortPosition.x + 500,
				viewPortPosition.y, viewPortPosition.y + 500);
	}

};

Vector para[4000];
struct IntersectionPoint {
	Vector intersection;
	Vector parabolaNormalVector;
	Vector splineNormalVector;
	Vector tangentOfSpline;

public:
	IntersectionPoint() {
		intersection = parabolaNormalVector = splineNormalVector = Vector();
		tangentOfSpline = Vector();
	}
};

IntersectionPoint intersection;
class Parabola {
	Vector normalVector;
public:

	Parabola() {

	}

	Vector getDirectionVector() {
		// Kiszamolom hogy mi az iranyvektora az egyenesnek, de ugy hogy mindig "jobbra mutasson"
		// Ha a kulonbseg vektor x koordinataja kisebb mint 0 akkor meg kell forditani
		if (pointsWithTime.SizeOf() >= 3) {
			Vector differentVector = pointsWithTime[0].controllPoint
					- pointsWithTime[1].controllPoint;
			if (differentVector.x < 0)
				differentVector = differentVector * (-1);
			return differentVector;
		}
		return Vector();
	}

	void calculateNormalVector() {
		// Normal vektor kiszamitasa, ugy hogy mindig felfele mutasson. Ezert kellett az iranyvektornak
		// Mindig jobbra mutatnia
		Vector direction = getDirectionVector();
		direction.Normalize();
		Vector normal = direction % Vector(0, 0, -1);
		normal.Normalize();
		normalVector = normal;
	}

	float distancePointFromLine() {
		// Elojelesen adja meg hogy az egyenestol mennyire van a pont, igy az eltolas
		// meghatarozasanal nem kell egyebb feltetelt vizsgalni
		return normalVector
				* (pointsWithTime[2].controllPoint
						- pointsWithTime[0].controllPoint);
	}

	float getRotationInRad() {
		// Kiszamolom a bezart szoget az iranyvektor es a  (1,0,0) vektor kozott
		// attol fuggoen hogy monoton novekvo vagy csokkeno az egyenes megvaltoztatom
		// 180 fokkot hozza adok ha csokken
		Vector dir = getDirectionVector();
		dir.Normalize();

		float rad = acos(Vector(1, 0, 0) * dir);

		float y1 = normalVector.x * (-1)
				* (500 - pointsWithTime[1].controllPoint.x);
		float y2 = y1 / normalVector.y;
		float Y1 = y2 + pointsWithTime[1].controllPoint.y;

		y1 = normalVector.x * (-1) * (550 - pointsWithTime[1].controllPoint.x);
		y2 = y1 / normalVector.y;
		float Y2 = y2 + pointsWithTime[1].controllPoint.y;

		if (Y1 > Y2) {
			rad = (PI_MAT - rad) + PI_MAT;
		}

		return rad;
	}

	Vector getTranslateVector() {
		// Megvan a normal vektor, mivel a tavolsag elojeles igy csak hozza kell adni a -1 szereset
		// a fokusz koordinatajahoz
		float dist = distancePointFromLine() / 2;
		Vector pushVector = normalVector * dist;

		pushVector = pushVector * (-1);

		pushVector = pointsWithTime[2].controllPoint + pushVector;

		return pushVector;
	}

	myMatrix calculateTranformationMatrix() {
		float rotation = getRotationInRad();

		Vector push = getTranslateVector();
		myMatrix translateMatrix = myMatrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
				push.x, push.y, 0, 1);
		myMatrix rotationMatrix = myMatrix(cos(rotation), sin(rotation), 0, 0,
				-sin(rotation), cos(rotation), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		myMatrix transformationMatrix = rotationMatrix * translateMatrix;
		return transformationMatrix;
	}

	Vector calculateTransformedPoint(float x) {
//		calculateNormalVector();

		float y;
		float p = distancePointFromLine();
		y = 1 / (2 * p) * powf(x, 2);
		Vector point = Vector(x, y);

		myMatrix transformationMatrix = calculateTranformationMatrix();
		Vector transformedPoint = point * transformationMatrix;

		return transformedPoint;
	}

	void printParabola() {
		int i = 0;
		if (pointsWithTime.SizeOf() >= 3) {
			calculateNormalVector();

//			glColor3f(0.255, 0.898, 0.969);
//						glBegin(GL_TRIANGLE_FAN);
//			for(int x=0; x<1000; x++){
//				for(int y=0; y<1000; y++){
//					glVertex2f(x, y);
//				}
//			}
//			glEnd();
			glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image2);

			glColor3f(1, 1, 0);
			glBegin(GL_TRIANGLE_FAN);
			for (float x = -1000; x < 1000; x = x + 0.5) {

				Vector point = calculateTransformedPoint(x);

////				cout<<normalVector.x<<" "<<normalVector.y<<endl;
//				float y1 = normalVector.x*(-1)*(x-pointsWithTime[1].controllPoint.x);
//				float y2 = y1 / normalVector.y;
//				float Y = y2 + pointsWithTime[1].controllPoint.y;
				para[i] = point;
//				glVertex2f(point.x, point.y);
				i++;

			}

			glEnd();
			glBegin(GL_TRIANGLE_FAN);
			for (int k = 0; k < i; k++) {
				glVertex2f(para[k].x, para[k].y);
			}
			glEnd();

		}

		if (pointsWithTime.SizeOf() >= 3) {
			calculateNormalVector();
			float rotation = getRotationInRad();
			Vector push = getTranslateVector();
//			glPushMatrix();
//			glTranslatef(push.x, push.y, 0);
//			glRotatef(180/PI_MAT*rotation, 0, 0, 1);

			glColor3f(0, 1, 0);
			glBegin(GL_LINE_STRIP);

			float m = 1 / (distancePointFromLine())
					* intersection.intersection.x;
			float y0 = 1 / (2 * distancePointFromLine())
					* powf(intersection.intersection.x, 2);

			for (float x = -1000; x <= 1000; x++) {

//				float y1 = normalVector.x * (-1)
//						* (x - pointsWithTime[1].controllPoint.x);
//				float y2 = y1 / normalVector.y;
//				float Y = y2 + pointsWithTime[1].controllPoint.y;

//				Y = -normalVector.x/normalVector.y;

				float p = distancePointFromLine();
				float y = 1 / (p) * x;
				float Y = m * (x) - y0;
				Vector point = Vector(x, Y);
				myMatrix transformationMatrix = calculateTranformationMatrix();
				point = point * transformationMatrix;
//				glVertex2f(point.x, point.y);

			}

			glEnd();
//			glPopMatrix();

		}

	}

};

Parabola parabola;

class TangentialToParabola {
public:
	void drawLine() {
		if (pointsWithTime.SizeOf() >= 3) {

			glColor3f(0, 0.7, 0);
			glBegin(GL_LINE_STRIP);

			Vector intersectionPoin = intersection.intersection;
			myMatrix mI = parabola.calculateTranformationMatrix().inverse();

			intersectionPoin = intersectionPoin * mI;

			float m = 1 / (parabola.distancePointFromLine())
					* intersectionPoin.x;
//			float y0 = 1 / (2 * parabola.distancePointFromLine())
//					* powf(intersectionPoin.x, 2);
			float y0 = intersectionPoin.y;

			for (float x = -1000; x <= 1000; x++) {

				//				float y1 = normalVector.x * (-1)
				//						* (x - pointsWithTime[1].controllPoint.x);
				//				float y2 = y1 / normalVector.y;
				//				float Y = y2 + pointsWithTime[1].controllPoint.y;

				//				Y = -normalVector.x/normalVector.y;

//				float p = distancePointFromLine();
//				float y = 1 / (p) * x;
				float Y = m * (x) - y0;
				Vector point = Vector(x, Y);
				myMatrix transformationMatrix =
						parabola.calculateTranformationMatrix();
				point = point * transformationMatrix;
				glVertex2f(point.x, point.y);

			}

			glEnd();
			//			glPopMatrix();

		}

	}
};

class TangentialToSpline {
public:
	void drawLine() {
		if (pointsWithTime.SizeOf() >= 3) {

			glColor3f(0, 0.7, 0);
			glBegin(GL_LINE_STRIP);

			Vector intersectionPoin = intersection.intersection;
			Vector dir = intersection.tangentOfSpline;

			for (float x = -1000; x <= 1000; x++) {

				float y1 = dir.y * intersectionPoin.x;
				float y2 = dir.x * intersectionPoin.y;
				float y3 = dir.y * x;
				float y4 = y1 - y2 - y3;
				float Y = y4 / (-dir.x);

				glVertex2f(x, Y);

			}

			glEnd();

		}

	}
};

bool ggg = true;
int jjj = 0;
class CatmullRom {
public:
	Array<ControllPointWithTime, 10> arrayOfPoints;

	CatmullRom(Array<ControllPointWithTime, 10> t) {
		arrayOfPoints = t;
	}
private:
	Vector velCatR(int n) {
		if (n == 0 || n == piecesOfControllPoints - 1)
			return Vector();

		long t = arrayOfPoints[n].time;
		long t_min = arrayOfPoints[n - 1].time;
		long t_plus = arrayOfPoints[n + 1].time;

		Vector tmp = arrayOfPoints[n + 1].controllPoint
				- arrayOfPoints[n].controllPoint;
		Vector first = tmp / ((t_plus - t));
		Vector tmp2 = (arrayOfPoints[n].controllPoint
				- arrayOfPoints[n - 1].controllPoint);
		Vector second = tmp2 / ((t - t_min));

		Vector acct;
		acct = ((first + second) * 0.5);

		return acct;
	}

	Vector a_2(int n) {

		Vector v = Vector();
		Vector v1;
		Vector v2;

		if (n == 0)
			v1 = v;
		else
			v1 = velCatR(n);
		if (n == piecesOfControllPoints - 1)
			v2 = v;
		else
			v2 = velCatR(n + 1);

		float timeDif = (arrayOfPoints[n + 1].time - arrayOfPoints[n].time);

		Vector first = (arrayOfPoints[n + 1].controllPoint
				- arrayOfPoints[n].controllPoint) * 3;
		float timeDifSq = powf(timeDif, (float) 2);

		Vector acct = ((first / timeDifSq) - ((v2 + v1 * 2) / timeDif));

		return acct;
	}

	Vector a_3(int n) {
		Vector v = Vector();
		Vector v1;
		Vector v2;

		if (n == 0)
			v1 = v;
		else
			v1 = velCatR(n);
		if (n == piecesOfControllPoints - 1)
			v2 = v;
		else
			v2 = velCatR(n + 1);

		float timeDif = (arrayOfPoints[n + 1].time - arrayOfPoints[n].time);

		Vector first = (arrayOfPoints[n].controllPoint
				- arrayOfPoints[n + 1].controllPoint) * 2;
		float timeDiffTr = powf(timeDif, (float) 3);
		float timeDiffSq = powf(timeDif, (float) 2);

		Vector acct = ((first / timeDiffTr) + ((v2 + v1) / timeDiffSq));

		return acct;
	}

	bool getIntersectionWithParabola(const Vector& splinePoint,
			Vector velocity) {
		bool notFound = true;

		for (int i = 0; i <= 4000; i++) {

			Vector parabolaPoint = para[i];
			if (fabs(parabolaPoint.y - splinePoint.y) <= 0.5
					&& fabs(parabolaPoint.x - splinePoint.x) <= 0.5 && notFound) {

				if (!space) {
					intersection.intersection = splinePoint;
					intersection.tangentOfSpline = velocity;
				}
				notFound = false;

				return true;
			}

		}
		return false;
	}

public:
	void drawCatmullR() {
		glColor3f(1, 1, 1);

		Vector vect;

		glBegin(GL_LINE_STRIP);

		float t;
		Vector v_3;
		Vector v_2;
		Vector v_1;
		bool b = false;

		for (int j = 0; j < piecesOfControllPoints - 1; ++j) {
//
//			for (t = arrayOfPoints[j].time; t <= arrayOfPoints[j + 1].time; t +=
//					1) {
			int step = 5;
			if (j == 1)
				step = 1;
			for (t = 0;
					t <= fabs(arrayOfPoints[j + 1].time - arrayOfPoints[j].time);
					t += step) {

//				float f3 = (powf(((t - arrayOfPoints[j].time)), (float) 3));
//
//				float f2 = (powf((t - arrayOfPoints[j].time), (float) 2));
//
//				v0 = a_3(j) * (f3);
//				v1 = a_2(j) * (f2);
//				v2 = velCatR(j) * (t - arrayOfPoints[j].time);
//
//				vect = (v_3 + v_2 + v_1 + arrayOfPoints[j].controllPoint);
//				glVertex2f(vect.x, vect.y);

				float f3 = (powf(((t)), (float) 3));

				float f2 = (powf((t), (float) 2));

				v_3 = a_3(j) * (f3);
				v_2 = a_2(j) * (f2);
				v_1 = velCatR(j) * (t);

				vect = (v_3 + v_2 + v_1 + arrayOfPoints[j].controllPoint);

				if (arrayOfPoints.SizeOf() >= 3 && !b && j == 1) {
					Vector p_3 = a_3(j) * f2 * 3;
					Vector p_2 = a_2(j) * t * 2;

					Vector velocity = p_3 + p_2 + velCatR(j);
					b = getIntersectionWithParabola(vect, velocity);

				}

				glVertex2f(vect.x, vect.y);

			}

		}
		if (piecesOfControllPoints > 1) {

			glVertex2f(arrayOfPoints[0].controllPoint.x,
					arrayOfPoints[0].controllPoint.y);
		}
		glEnd();

	}

};
Animation animate = Animation();
void addControlPointToArray(int x, int y, long time) {
	ControllPointWithTime controllPoint;
	Vector center;
	if (!space) {
		center.x = (float) x * ((float) 1000 / screenWidth);
		center.y = 1000 - y * ((float) 1000 / screenHeight);
	}else {
		center.x = ((float) x * ((float) 500 / screenWidth))
				+ animate.viewPortPosition.x;
		center.y = (500 - y * ((float) 500 / screenHeight))
				+ animate.viewPortPosition.y;
	}

	controllPoint.controllPoint = center;
	controllPoint.time = time;
	pointsWithTime.Add(controllPoint);

	piecesOfControllPoints++;
}

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization() {
	glViewport(0, 0, screenWidth, screenHeight);

	gluOrtho2D(0.0, (float) 1000, 0.0, (float) 1000);

	// Peldakent keszitunk egy kepet az operativ memoriaba
	for (int Y = 0; Y < screenHeight; Y++)
		for (int X = 0; X < screenWidth; X++)
			image[Y * screenWidth + X] = Color(0, 0, 0);
	for (int Y = 0; Y < screenHeight; Y++)
		for (int X = 0; X < screenWidth; X++)
			image2[Y * screenWidth + X] = Color(0.255, 0.898, 0.969);

}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay() {
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

	// ..

	// Peldakent atmasoljuk a kepet a rasztertarba
	glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);
	// Majd rajzolunk egy kek haromszoget

	// ...

	parabola.printParabola();
	CatmullRom c = CatmullRom(pointsWithTime);
	c.drawCatmullR();

	for (int i = 0; i < piecesOfControllPoints; i++) {
		Circle c = Circle(pointsWithTime[i].controllPoint, 5);
		c.drawOut();
	}

	TangentialToParabola tangentialToParabola;
	tangentialToParabola.drawLine();
	TangentialToSpline tangentialToSpline;
	tangentialToSpline.drawLine();
	Circle cc = Circle(intersection.intersection, 10);
//	cc.drawOut();
	animate.animation();
	glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
	if (key == ' ' && !space) {
		animate.resizeViewPort();
		space = true;
	}
	glutPostRedisplay(); 		// d beture rajzold ujra a kepet

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN
			&& pointsWithTime.SizeOf() < 9) { // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		long time = glutGet(GLUT_ELAPSED_TIME);
		addControlPointToArray(x, y, time);
	}
	glutPostRedisplay(); 					// Ilyenkor rajzold ujra a kepet
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y) {

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME);	// program inditasa ota eltelt ido
	if (space)
		glutPostRedisplay();

}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
	glutInit(&argc, argv); 				// GLUT inicializalasa
	glutInitWindowSize(600, 600); // Alkalmazas ablak kezdeti merete 600x600 pixel
	glutInitWindowPosition(100, 100); // Az elozo alkalmazas ablakhoz kepest hol tunik fel
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // 8 bites R,G,B,A + dupla buffer + melyseg buffer

	glutCreateWindow("Grafika hazi feladat"); // Alkalmazas ablak megszuletik es megjelenik a kepernyon

	glMatrixMode(GL_MODELVIEW);	// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();

	onInitialization();				// Az altalad irt inicializalast lefuttatjuk

	glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();					// Esemenykezelo hurok

	return 0;
}
