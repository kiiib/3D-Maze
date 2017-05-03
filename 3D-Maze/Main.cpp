#include "Angel.h"
#include <iostream>
#include <stack>
using namespace std;

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;
const int numVertices = 1600;
const int numOfCubePoints = 8;
const int WIDTH = 500;
const int HEIGHT = 500;
const int colCount = 10;
const int rowCount = 10;
int mazeCount = colCount * rowCount;

point4 points[numVertices];
color4 colors[numVertices];

color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};

class Cell {
public:
	int numCell;
	bool wallTop = true;
	bool wallRight = true;
	bool wallBottom = true;
	bool wallLeft = true;
	bool visited = false;
};

int index = 0;
void quad(int a, int b, int c, int d, point4 vertices[numOfCubePoints]) {
	colors[index] = vertex_colors[a];
	points[index] = vertices[a];
	index++;
	colors[index] = vertex_colors[b];
	points[index] = vertices[b];
	index++;
	colors[index] = vertex_colors[c];
	points[index] = vertices[c];
	index++;
	colors[index] = vertex_colors[a];
	points[index] = vertices[a];
	index++;
	colors[index] = vertex_colors[c];
	points[index] = vertices[c];
	index++;
	colors[index] = vertex_colors[d];
	points[index] = vertices[d];
	index++;

}
vector<Cell> mazeArray;
Cell currentCell;
stack<Cell> cellPath;	// for recording the modification of current cell into stack

point4* getBoxPoints(int i) {
	const float startPointX = 1;
	const float startPointY = 1;
	
	point4 vertices[numOfCubePoints] = {
		point4(),
	};
	
}
void init() {
	for (int i = 0; i < mazeCount; i++) {
		Cell cell;
		mazeArray[i] = cell;

		point4* vertices = getBoxPoints(i);
		if (!mazeArray[i].wallRight)
			quad(1, 0, 4, 5, vertices);
	}


}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
	glutPostRedisplay();
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
}

//按下 上、下、左、右 會觸發的事件
void specialInput(int key, int x, int y) {

	glutPostRedisplay();
}

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1366, 768);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Cube");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialInput);
	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}