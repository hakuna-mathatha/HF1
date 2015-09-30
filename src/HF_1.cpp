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
// 3D Vektor
//--------------------------------------------------------
struct Vector {
	float x, y, z;

	Vector() {
		x = y = z = 0;
	}
	Vector(float x0, float y0, float z0 = 0) {
		x = x0;
		y = y0;
		z = z0;
	}
	Vector operator*(float a) {
		return Vector(x * a, y * a, z * a);
	}
	Vector operator/(float a) {
		return Vector(x / a, y / a, z / a);
	}
	Vector operator+(const Vector& v) {
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v) {
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	float operator*(const Vector& v) { 	// dot product
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector operator%(const Vector& v) { 	// cross product
		return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
	float Length() {
		return sqrt(x * x + y * y + z * z);
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
		}
	}
};

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
public:
	void drawCatmullR() {
		glColor3f(1, 1, 1);
		//glLineWidth(2.5);

		Vector vect;

		glBegin(GL_LINE_STRIP);

		float t;
		Vector v0;
		Vector v1;
		Vector v2;

		for (int j = 0; j < piecesOfControllPoints - 1; ++j) {
//
//			for (t = arrayOfPoints[j].time; t <= arrayOfPoints[j + 1].time; t +=
//					1) {

			for (t = 0;
					t <= fabs(arrayOfPoints[j + 1].time - arrayOfPoints[j].time);
					t += 1) {

//				float f3 = (powf(((t - arrayOfPoints[j].time)), (float) 3));
//
//				float f2 = (powf((t - arrayOfPoints[j].time), (float) 2));
//
//				v0 = a_3(j) * (f3);
//				v1 = a_2(j) * (f2);
//				v2 = velCatR(j) * (t - arrayOfPoints[j].time);
//
//				vect = (v0 + v1 + v2 + arrayOfPoints[j].controllPoint);
//				glVertex2f(vect.x, vect.y);

				float f3 = (powf(((t)), (float) 3));

				float f2 = (powf((t), (float) 2));

				v0 = a_3(j) * (f3);
				v1 = a_2(j) * (f2);
				v2 = velCatR(j) * (t);

				vect = (v0 + v1 + v2 + arrayOfPoints[j].controllPoint);
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
bool space = false;

class Animation {
	Vector viewPortPosition;
	long animationTime;
	Vector myDir;

	bool moveLeft;
	bool moveRight;

public:

	Animation() {
		moveLeft = true;
		moveRight = false;
		animationTime = 0;
		viewPortPosition.x = 150;
		viewPortPosition.y = 150;
		myDir = Vector();

	}

	void randomBeginingDir() {
		long time = glutGet(GLUT_ELAPSED_TIME);
		float randomX = time % 17;
		float randomY = time % 11;
		cout << randomX << "  " << randomY << endl;
		myDir = Vector(randomX, randomY);
		myDir.Normalize();
	}

	Vector getNormalOfTheWall() {
		if (viewPortPosition.x <= 0) {
			return Vector(1, 0);
		} else if (viewPortPosition.x >= 300) {
			return Vector(-1, 0);
		} else if (viewPortPosition.y <= 0) {
			return Vector(0, 1);
		} else if (viewPortPosition.y >= 300) {
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
		if (time - animationTime > 5 && space) {

			animationTime = glutGet(GLUT_ELAPSED_TIME);

			myDir = reflect();

			viewPortPosition.x = myDir.x * 2 + viewPortPosition.x;
			viewPortPosition.y = myDir.y * 2 + viewPortPosition.y;
			glViewport(viewPortPosition.x, viewPortPosition.y, 300, 300);

		}
	}

	void resizeViewPort() {
		animationTime = glutGet(GLUT_ELAPSED_TIME);
		space = true;
		glViewport(viewPortPosition.x, viewPortPosition.y, 300, 300);
	}

};

class Parabola {
	Vector normalVector;
public:

	Parabola() {

	}

	Vector getDirectionVector() {
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
		Vector direction = getDirectionVector();
		direction.Normalize();
		Vector normal = direction % Vector(0, 0, -1);
		normal.Normalize();
		normalVector = normal;
	}

	float distancePointFromLine() {
		return normalVector
				* (pointsWithTime[2].controllPoint
						- pointsWithTime[0].controllPoint);
	}

	float getRotationInDegree() {
		Vector dir = getDirectionVector();
		dir.Normalize();
//		cout<<"Dir: x "<<dir.x<<" y "<<dir.y<<endl;
//		cout<<"Norm: x "<<normalVector.x<<" y "<<normalVector.y<<endl;

		float rad = acos( Vector(1, 0, 0)*dir);
//		cout<<"Rotation: "<<rad<<endl;
		float y1 = normalVector.x * (-1)
				* (500 - pointsWithTime[1].controllPoint.x);
		float y2 = y1 / normalVector.y;
		float Y1 = y2 + pointsWithTime[1].controllPoint.y;

		y1 = normalVector.x * (-1) * (550 - pointsWithTime[1].controllPoint.x);
		y2 = y1 / normalVector.y;
		float Y2 = y2 + pointsWithTime[1].controllPoint.y;

		if (Y1 > Y2) {
			rad = (PI_MAT - rad)+PI_MAT;
		}

		float deg = 180 / PI_MAT * rad;
		return deg;
	}

	void printParabola() {
		if (pointsWithTime.SizeOf() >= 3) {
			calculateNormalVector();
			float rotation = getRotationInDegree();
			cout << "Rotation: " << rotation << endl;

			glPushMatrix();
			glTranslatef(pointsWithTime[2].controllPoint.x, pointsWithTime[2].controllPoint.y, 0);
			glRotatef(rotation, 0, 0, 1);

			glColor3f(0, 1, 0);
			glBegin(GL_LINE_STRIP);
			for (float x = -1000; x < 1000; x++) {
				float X = x;
				float y;
				float p = distancePointFromLine();
				y = 1 / (2 * p) * powf(x, 2);
				float Y = y;
//

////				cout<<normalVector.x<<" "<<normalVector.y<<endl;
//				float y1 = normalVector.x*(-1)*(x-pointsWithTime[1].controllPoint.x);
//				float y2 = y1 / normalVector.y;
//				float Y = y2 + pointsWithTime[1].controllPoint.y;
				glVertex2f(X, Y);

			}

			glEnd();
			glPopMatrix();

		}


		if (pointsWithTime.SizeOf() >= 3) {
					calculateNormalVector();
					float rotation = getRotationInDegree();
					cout << "Rotation: " << rotation << endl;

					glPushMatrix();
//					glTranslatef(pointsWithTime[2].controllPoint.x, pointsWithTime[2].controllPoint.y, 0);
//					glRotatef(rotation, 0, 0, 1);

					glColor3f(1, 1, 0);
					glBegin(GL_LINE_STRIP);
					for (float x = -1000; x < 1000; x++) {
//						float X = x;
//						float y;
//						float p = distancePointFromLine();
//						y = 1 / (2 * p) * powf(x, 2);
//						float Y = y;
		//

		//				cout<<normalVector.x<<" "<<normalVector.y<<endl;
						float y1 = normalVector.x*(-1)*(x-pointsWithTime[1].controllPoint.x);
						float y2 = y1 / normalVector.y;
						float Y = y2 + pointsWithTime[1].controllPoint.y;
						glVertex2f(x, Y);

					}

					glEnd();
					glPopMatrix();

				}

	}

};

void addControlPointToArray(int x, int y, long time) {
	ControllPointWithTime controllPoint;
	Vector center;
	center.x = (float) x * ((float) 1000 / screenWidth);
	center.y = 1000 - y * ((float) 1000 / screenHeight);
	controllPoint.controllPoint = center;
	controllPoint.time = time;
	pointsWithTime.Add(controllPoint);

	piecesOfControllPoints++;
}

Animation animate = Animation();
Parabola parabola;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization() {
	glViewport(0, 0, screenWidth, screenHeight);

	gluOrtho2D(0.0, (float) 1000, 0.0, (float) 1000);

	// Peldakent keszitunk egy kepet az operativ memoriaba
	for (int Y = 0; Y < screenHeight; Y++)
		for (int X = 0; X < screenWidth; X++)
			image[Y * screenWidth + X] = Color(0.6, 0.6, 0.6);

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
//	Circle c = Circle(Vector(500, 500, 0), 50);
//	c.drawOut();

	animate.animation();

	CatmullRom c = CatmullRom(pointsWithTime);
	c.drawCatmullR();

	for (int i = 0; i < piecesOfControllPoints; i++) {
		Circle c = Circle(pointsWithTime[i].controllPoint, 5);
		c.drawOut();
	}
//	glPushMatrix();
////glTranslatef(250, 0,0);
//	glTranslatef(500, 500, 0);
//
//	glRotatef(-45, 0, 0, 1);
//
	parabola.printParabola();
//	glPopMatrix();

	glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
	if (key == ' ' && !space) {
		animate.resizeViewPort();
		animate.randomBeginingDir();
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
