#include "Angel.h"
typedef Angel::vec4 color4;
typedef Angel::vec4 point4;
const int WIDTH = 500;
const int HEIGHT = 500;
const int colCount = 10;
const int rowCount = 10;
int maze_area = colCount * rowCount;
const int numVertices = 1600;
const int numOfCubePoints = 8;

point4 points[numVertices];
color4 colors[numVertices];


void init() {
	const float start_point_x = 1;
	const float start_point_y = 1;
}

void display() {

}

void keyboard(unsigned char key, int x, int y) {

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