#include "Angel.h"
#include <iostream>
#include <stack>
using namespace std;

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;
const int numVertices = 1600;
const int numOfCubePoints = 8;
const int WINDOW_WIDTH = 1366;
const int WINDOW_HEIGHT = 768;
const int colCount = 10;
const int rowCount = 10;
int mazeCount = colCount * rowCount;

float up_down = 0;
float playerLookAtX = 0;
float playerLookAtZ = 0;
GLuint bufferMaze;
point4 eye(1, 1.5, 1, 1.0);
float angleNumber = 45;
GLuint vaoMaze;

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

GLfloat radius = 1.0;
GLfloat theta = 0.0;
GLfloat phi = 0.0;

const GLfloat  dr = 5.0 * DegreesToRadians;
GLuint  model_view;
GLfloat  left = -1.0, right = 1.0;
GLfloat  bottom = -1.0, top = 1.0;
GLfloat  zNear = 0.5, zFar = 100.0;

GLuint  projection;

int mazeIndex(int x, int y) {
	if (x < 0 || y < 0 || x > colCount - 1 || y > rowCount - 1)
		return -1;

	return x + y * rowCount;	// depents on row count number, such as rowCount = 10, maze: 10x10
}

class Cell {
public:
	int numCell;
	int x;
	int y;
	bool wallTop = true;
	bool wallRight = true;
	bool wallBottom = true;
	bool wallLeft = true;
	bool visited = false;
	bool isExist = true;

	Cell checkNeighbors(vector<Cell> myMaze) {
		vector<Cell> neighbors;

		if (x - 1 >= 0) {
			Cell top = myMaze[mazeIndex(x, y - 1)];
			if (!top.visited) {
				neighbors.push_back(top);
			}
		}
		if (x + 1 < colCount) {
			Cell right = myMaze[mazeIndex(x + 1, y)];
			if (!right.visited) {
				neighbors.push_back(right);
			}
		}
		if (y + 1 < rowCount) {
			Cell bottom = myMaze[mazeIndex(x, y + 1)];
			if (!bottom.visited) {
				neighbors.push_back(bottom);
			}
		}
		if (x - 1 >= 0) {
			Cell left = myMaze[mazeIndex(x - 1, y)];
			if (!left.visited) {
				neighbors.push_back(left);
			}
		}
		// if neighors are exist, pick up a random neigbor
		if (neighbors.size() > 0) {
			int random = (floor)(rand() % neighbors.size());
			return neighbors[random];
		}
		else {
			// if neighbor doesn't exists, return none
			Cell nullCell;
			nullCell.isExist = false;
			return nullCell;
		}
	}

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
vector<Cell> maze;
Cell currentCell;
stack<Cell> cellPath;	// for recording the modification of current cell into stack

void removeWalls(Cell currentCell, Cell neighborCell) {
	int x = currentCell.x - neighborCell.x;
	int y = currentCell.y - neighborCell.y;

	/*
	x = 1 means neighbor in the left side
	remove the current cell's left wall and neighbor's right wall

	x = -1 means neighbor in the right side
	remove the current cell's right wall and neighbor's left wall

	y = 1 means neighbor in the top side
	remove the current cell's top wall and neighbor's bottom wall

	y = -1 means neighbor in the bottom side
	remove the current cell's bottom wall and neighbor's top wall
	*/
	if (x == 1) {
		maze[currentCell.numCell].wallLeft = false;
		maze[neighborCell.numCell].wallRight = false;
	}
	if (x == -1) {
		maze[currentCell.numCell].wallRight = false;
		maze[neighborCell.numCell].wallLeft = false;
	}
	if (y == 1) {
		maze[currentCell.numCell].wallTop = false;
		maze[neighborCell.numCell].wallBottom = false;
	}
	if (y == -1) {
		maze[currentCell.numCell].wallBottom = false;
		maze[neighborCell.numCell].wallTop = false;
	}
}

// OpenGL initialization
point4* getBoxPoints(int i) {
	const float startPointX = 1;
	const float startPointY = 1;
	float mazeRatio = 2;
	int col = i % colCount;
	int row = (i - col) / rowCount;
	point4 vertices[numOfCubePoints] = {
		point4(startPointX + col * mazeRatio,mazeRatio * 0.5, startPointY + row * mazeRatio,   1.0),
		point4(startPointX + col * mazeRatio, mazeRatio *  0.5, startPointY + row * mazeRatio + mazeRatio,  1.0),
		point4(startPointX + col * mazeRatio + mazeRatio, mazeRatio *  0.5, startPointY + row * mazeRatio + mazeRatio,  1.0),
		point4(startPointX + col * mazeRatio + mazeRatio, mazeRatio *  0.5, startPointY + row * mazeRatio,  1.0),
		point4(startPointX + col * mazeRatio,mazeRatio * -0.5, startPointY + row * mazeRatio,  1.0),
		point4(startPointX + col * mazeRatio, mazeRatio * -0.5, startPointY + row * mazeRatio + mazeRatio,  1.0),
		point4(startPointX + col * mazeRatio + mazeRatio, mazeRatio *  -0.5, startPointY + row * mazeRatio + mazeRatio, 1.0),
		point4(startPointX + col * mazeRatio + mazeRatio, mazeRatio *  -0.5, startPointY + row * mazeRatio, 1.0)
	};
	return vertices;
}

void generateMaze() {
	int numCell = 0;
	// generate the maze's each cell
	for (int j = 0; j < rowCount; j++) {
		for (int i = 0; i < colCount; i++) {
			Cell cell;
			cell.x = j;
			cell.y = i;
			cell.numCell = numCell;
			// push each cell into grid
			maze.push_back(cell);
			numCell++;
		}
	}
}
void drawMaze() {
	currentCell = maze[0];	// set the first cell is current
	maze[currentCell.numCell].visited = true;

	do {
		Cell nextCell = currentCell.checkNeighbors(maze);
		if (nextCell.isExist) {
			// mark the next cell is visited
			maze[nextCell.numCell].visited = true;
			// push the current cell into stack
			cellPath.push(currentCell);

			// remove the wall between the current cell and the chosen cell
			removeWalls(currentCell, nextCell);

			// set it to be the next one
			currentCell = nextCell;
		}
		else if (!nextCell.isExist) {
			// confirm the stack is not empty and pop an element(the lastest)
			cellPath.pop();
			currentCell = cellPath.top();	// get the last cell
		}
	} while (currentCell.numCell != 0);	// until back to the start site, stop the loop

	for (int i = 0; i < maze.size(); i++) {
		point4* vertices = getBoxPoints(i);

		if (maze[i].wallRight)
			quad(1, 0, 4, 5, vertices);
		if (maze[i].wallTop)
			quad(3, 0, 4, 7, vertices);
		if (maze[i].wallLeft)
			quad(2, 3, 7, 6, vertices);
		if (maze[i].wallBottom)
			quad(6, 5, 1, 2, vertices);
	}
}
void initMaze() {
	srand((unsigned)time(NULL));
	generateMaze();
	drawMaze();

	glGenVertexArrays(1, &vaoMaze);
	glBindVertexArray(vaoMaze);
	glGenBuffers(1, &bufferMaze);
	glBindBuffer(GL_ARRAY_BUFFER, bufferMaze);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	GLuint program = InitShader("vshader41.glsl", "fshader41.glsl");
	glUseProgram(program);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}
int cameraAtWhichWall(point4 eye) {

	for (int i = 0; i < mazeCount; i++) {
		point4 *box = getBoxPoints(i);
		int minX = 50000; // 假設不超過@@...
		int minZ = 50000;// 假設不超過@@...

		int maxX = -minX; // 假設不超過@@...
		int maxZ = -minZ; // 假設不超過@@...

		for (int j = 0; j < numOfCubePoints; j++) {
			if (box[j].x > minX) {
				maxX = box[j].x;
			}

			if (box[j].z > minZ) {
				maxZ = box[j].z;
			}

			if (box[j].x < minX) {
				minX = box[j].x;
			}

			if (box[j].z < minZ) {
				minZ = box[j].z;
			}
		}

		if (minX <= eye.x && eye.x <= maxX && minZ <= eye.z && eye.z <= maxZ) {
			return i;
		}
	}

	return -1;
}
//按下 上、下、左、右 會觸發的事件
void specialInput(int key, int x, int y) {
	float step = 1.5;
	switch (key) {
	case GLUT_KEY_UP:
		up_down = 0.2;
		break;
	case GLUT_KEY_DOWN:
		up_down = -0.2;
		break;
	case GLUT_KEY_LEFT:
		angleNumber -= 1;
		//do something here
		break;
	case GLUT_KEY_RIGHT:
		angleNumber += 1;
		break;
	}

	glutPostRedisplay();
}
void display() {
	double angle = angleNumber * M_PI / 180;
	//移動下一個點事前的宣告
	point4 nextEye(eye.x, eye.y, eye.z, eye.w);
	//為了做到整個 Camera不是完全貼牆
	//所以我們用了nextNextEye去跟牆之間保持點距離 才不會等到 Camera完全貼到牆上才發生碰撞(這樣視線比較好)
	//如果有按下上或下 up_down才會變化 如果是正的就是往前，負的就是往後，記得用完後要設定回0喔
	point4 nextNextEye(eye.x, eye.y, eye.z, eye.w);
	nextEye.x = eye.x + cos(angle) * up_down;
	nextEye.z = eye.z + sin(angle) * up_down;
	nextNextEye.x = eye.x + cos(angle) * up_down * 5;
	nextNextEye.z = eye.z + sin(angle) * up_down * 5;
	up_down = 0;

	//取得目前的座標 跟 未來的座標，所在迷宮的位置
	int cameraCurrentIndex = cameraAtWhichWall(eye);
	int cameraFutureIndex = cameraAtWhichWall(nextNextEye);

	// std::cout << "CAMERA AT: " << cameraCurrentIndex << std::endl;
	// std::cout << "FUTURE CAMERA AT: " << cameraFutureIndex << std::endl;

	//開始處理碰撞事件
	//switch (cameraFutureIndex - cameraCurrentIndex) {
	//case 0:
	//	eye = nextEye;
	//	break;
	//case -1: // RIGHT
	//	if (((maze_array[cameraCurrentIndex] >> 3) & 1) != 0 && ((maze_array[cameraFutureIndex] >> 1) & 1) != 0) {
	//		eye = nextEye;
	//	}

	//	break;
	//case -10: // BOTTOM
	//	if (((maze_array[cameraCurrentIndex] >> 2) & 1) != 0 && (maze_array[cameraFutureIndex] & 1) != 0) {
	//		eye = nextEye;
	//	}

	//	break;
	//case 1: // LEFT
	//	if (((maze_array[cameraCurrentIndex] >> 1) & 1) != 0 && ((maze_array[cameraFutureIndex] >> 3) & 1) != 0) {
	//		eye = nextEye;
	//	}

	//	break;
	//case 10: // TOP
	//	if ((maze_array[cameraCurrentIndex] & 1) != 0 && ((maze_array[cameraFutureIndex] >> 2) & 1) != 0) {
	//		eye = nextEye;
	//	}

	//	break;
	//}


	playerLookAtX = eye.x + cos(angle) * 10;
	playerLookAtZ = eye.z + sin(angle) * 10;

	// std::cout << eye.x << ", " << eye.y << ", " << eye.z << ", " << angleNumber << std::endl;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	point4  at(playerLookAtX, 0.0, playerLookAtZ, 1.0);
	vec4    up(0.0, 1.0, 0.0, 0.0);

	mat4  mv = LookAt(eye, at, up);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	mat4  p = Perspective(45, (float)WINDOW_WIDTH / WINDOW_HEIGHT, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glBindVertexArray(bufferMaze);
	glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glEnable(GL_LIGHT0);
	glutSwapBuffers();
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
int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Cube");

	glewInit();

	initMaze();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialInput);
	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}