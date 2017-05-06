/* 3D Maze */
#include "Angel.h"
#include <vector>
#include <stack>
using namespace std;
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
const int NumVertices = 1600;
const int NumOfCubePoints = 8;
const int WIDTH = 1366;
const int HEIGHT = 768;
const int col_count = 10;
const int row_count = 10;
int maze_area = col_count * row_count;
int *maze_array = (int *)malloc(col_count * row_count * sizeof(int *));
float up_down = 0;
float playerLookAtX = 0;
float playerLookAtZ = 0;
GLuint bufferMaze;
point4 eye(2, 1.5, 2, 1.0);
float angleNumber = 45;
GLuint vaoMaze;
int HitTime = 0;
point4 points[NumVertices];
color4 colors[NumVertices];

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
GLfloat  leftview = -1.0, rightview = 1.0;
GLfloat  bottom = -1.0, top = 1.0;
GLfloat  zNear = 0.5, zFar = 100.0;

GLuint  projection;

int Index = 0;
int PlayerIndex = 0;

int index(int coordinateX, int coordinateY) {
	//figure out the invalid index
	if (coordinateX < 0 || coordinateY < 0 || coordinateX > col_count - 1 || coordinateY > row_count - 1)
		return -1;

	return coordinateX + coordinateY * 10;	// 10, because of there is a 10x10 grid
}

class Cell {
public:
	int numCell;
	int coordinateX;	// the location of coordinate
	int coordinateY;
	bool wallTop = true;
	bool wallRight = true;
	bool wallBottom = true;
	bool wallLeft = true;
	bool visited = false;
	bool isExist = true;

	Cell checkNeighbors(vector<Cell> myGrid) {
		vector<Cell> neighbors;
		//cout << coordinateX <<  "  "  << coordinateY - 1 << endl;

		//figure out the edge problem
		if (coordinateY - 1 >= 0) {
			Cell top = myGrid[index(coordinateX, coordinateY - 1)];
			if (!top.visited) {
				neighbors.push_back(top);
			}
		}
		if (coordinateX + 1 < col_count) {
			Cell right = myGrid[index(coordinateX + 1, coordinateY)];
			if (!right.visited) {
				neighbors.push_back(right);
			}
		}

		if (coordinateY + 1 < row_count) {
			Cell bottom = myGrid[index(coordinateX, coordinateY + 1)];
			if (!bottom.visited) {
				neighbors.push_back(bottom);
			}
		}

		if (coordinateX - 1 >= 0) {
			Cell left = myGrid[index(coordinateX - 1, coordinateY)];
			if (!left.visited) {
				neighbors.push_back(left);
			}
		}
		// if neighors are exist, pick up a random neigbor
		if (neighbors.size() > 0) {

			int random = (floor)(rand() % neighbors.size());
			//cout << "chosen neighbor is " << random << endl;
			return neighbors[random];
		}
		else {
			Cell nullCell;
			nullCell.isExist = false;
			return nullCell;	// if neighbor doesn't exists, return nothing
		}
	}
};
vector<Cell> grid;
Cell currentCell;
stack<Cell> cellPath;	// for recording the modification of current cell into the stack

void removeWalls(Cell currentCell, Cell neighborCell) {
	int x = currentCell.coordinateX - neighborCell.coordinateX;
	int y = currentCell.coordinateY - neighborCell.coordinateY;

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
		grid[currentCell.numCell].wallLeft = false;
		grid[neighborCell.numCell].wallRight = false;
	}
	if (x == -1) {
		grid[currentCell.numCell].wallRight = false;
		grid[neighborCell.numCell].wallLeft = false;
	}
	if (y == 1) {
		grid[currentCell.numCell].wallTop = false;
		grid[neighborCell.numCell].wallBottom = false;
	}
	if (y == -1) {
		grid[currentCell.numCell].wallBottom = false;
		grid[neighborCell.numCell].wallTop = false;
	}

}
void initGrid() {
	int numCell = 0;
	// init grid
	for (int j = 0; j < col_count; j++) {
		for (int i = 0; i < row_count; i++) {
			Cell cell;
			cell.coordinateX = i;
			cell.coordinateY = j;
			//cout << cell.coordinateX << "   " << cell.coordinateY << endl;
			cell.numCell = numCell;
			// push each cell into grid
			grid.push_back(cell);
			//cout << cell.numCell << endl;
			numCell++;
		}
	}
}
void drawGrid() {
	currentCell = grid[0];	// set the first cell is current
	grid[currentCell.numCell].visited = true;

	do {
		Cell nextCell = currentCell.checkNeighbors(grid);
		if (nextCell.isExist) {
			//cout << "next cell is " << nextCell.numCell << endl;

			//mark the next cell is visited
			grid[nextCell.numCell].visited = true;
			// push the current cell into stack
			cellPath.push(currentCell);

			// remove the wall between the current cell and the chosen cell
			removeWalls(currentCell, nextCell);

			currentCell = nextCell;	// set it to be the next one
			//cout << "current Cell " << currentCell.numCell << endl;
		}
		else if (!nextCell.isExist) {
			// confirm the stack is not empty and pop an element(the lastest)
			cellPath.pop();
			currentCell = cellPath.top();	// get the last cell
		}
	} while (currentCell.numCell != 0);	// until back to the start site stop the loop


	for (int i = 0; i < grid.size(); i++) {
		maze_array[i] = 0b11111;
		if (grid[i].wallLeft)
			maze_array[i] &= 0b10111;
		if (grid[i].wallTop)
			maze_array[i] &= 0b11011;
		if (grid[i].wallRight)
			maze_array[i] &= 0b11101;
		if (grid[i].wallBottom)
			maze_array[i] &= 0b11110;
	}

}
void
quad(int a, int b, int c, int d, point4 vertices[NumOfCubePoints])
{
	colors[Index] = vertex_colors[a]; 
	points[Index] = vertices[a]; 
	Index++;
	colors[Index] = vertex_colors[b]; 
	points[Index] = vertices[b]; 
	Index++;
	colors[Index] = vertex_colors[c]; 
	points[Index] = vertices[c]; 
	Index++;
	colors[Index] = vertex_colors[a]; 
	points[Index] = vertices[a]; 
	Index++;
	colors[Index] = vertex_colors[c]; 
	points[Index] = vertices[c]; 
	Index++;
	colors[Index] = vertex_colors[d]; 
	points[Index] = vertices[d]; 
	Index++;

	 //std::cout << Index << std::endl;
}

// OpenGL initialization
point4* getBoxPoints(int i) {
	const float start_point_x = 1;
	const float start_point_y = 1;
	float maze_radio = 2;
	int col = i % col_count;
	int row = (i - col) / row_count;
	//std::cout << col << "   " << row << std::endl;
	point4 vertices[NumOfCubePoints] = {
		point4(start_point_x + col * maze_radio,maze_radio * 0.5, start_point_y + row * maze_radio,   1.0),
		point4(start_point_x + col * maze_radio, maze_radio *  0.5, start_point_y + row * maze_radio + maze_radio,  1.0),
		point4(start_point_x + col * maze_radio + maze_radio, maze_radio *  0.5, start_point_y + row * maze_radio + maze_radio,  1.0),
		point4(start_point_x + col * maze_radio + maze_radio, maze_radio *  0.5, start_point_y + row * maze_radio,  1.0),
		point4(start_point_x + col * maze_radio,maze_radio * -0.5, start_point_y + row * maze_radio,  1.0),
		point4(start_point_x + col * maze_radio, maze_radio * -0.5, start_point_y + row * maze_radio + maze_radio,  1.0),
		point4(start_point_x + col * maze_radio + maze_radio, maze_radio *  -0.5, start_point_y + row * maze_radio + maze_radio, 1.0),
		point4(start_point_x + col * maze_radio + maze_radio, maze_radio *  -0.5, start_point_y + row * maze_radio, 1.0)
	};
	//std::cout << vertices << std::endl;
	return vertices;
}

void init()
{
	// create the wall 
	for (int i = 0; i < grid.size(); i++) {

		point4* vertices = getBoxPoints(i);

		if (grid[i].wallLeft) {
			quad(1, 0, 4, 5, vertices);
		}
		if (grid[i].wallTop) {
			quad(3, 0, 4, 7, vertices);
		}
		if (grid[i].wallRight) {
			quad(2, 3, 7, 6, vertices);
		}

		if (grid[i].wallBottom) {		
			quad(6, 5, 1, 2, vertices);
		}
	}

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

	for (int i = 0; i < grid.size(); i++) {
		point4 *box = getBoxPoints(i);
		int minX = 50000; // 假設不超過@@...
		int minZ = 50000;// 假設不超過@@...

		int maxX = -minX; // 假設不超過@@...
		int maxZ = -minZ; // 假設不超過@@...

		for (int j = 0; j < NumOfCubePoints; j++) {
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
//----------------------------------------------------------------------------

void
display(void)
{
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
	 //std::cout << "FUTURE CAMERA AT: " << cameraFutureIndex << std::endl;

	
	//開始處理碰撞事件
	switch (cameraFutureIndex - cameraCurrentIndex) {
	case 0:
		eye = nextEye;
		break;
	case -1: // RIGHT
		
		if (((maze_array[cameraCurrentIndex] >> 3) & 1) != 0 && ((maze_array[cameraFutureIndex] >> 1) & 1) != 0) {
			eye = nextEye;
			
		}
		else {
			std::cout << "Hit wall " << HitTime << " Times" << std::endl;
			HitTime++;
		}

		break;
	case -10: // BOTTOM
		
		if (((maze_array[cameraCurrentIndex] >> 2) & 1) != 0 && (maze_array[cameraFutureIndex] & 1) != 0) {
			eye = nextEye;
			
		}
		else {
			std::cout << "Hit wall " << HitTime << " Times" << std::endl;
			HitTime++;
		}

		break;
	case 1: // LEFT
		
		if (((maze_array[cameraCurrentIndex] >> 1) & 1) != 0 && ((maze_array[cameraFutureIndex] >> 3) & 1) != 0) {
			eye = nextEye;
		}
		else {
			std::cout << "Hit wall " << HitTime << " Times" << std::endl;
			HitTime++;
		}

		break;
	case 10: // TOP
		
		if ((maze_array[cameraCurrentIndex] & 1) != 0 && ((maze_array[cameraFutureIndex] >> 2) & 1) != 0) {
			eye = nextEye;
			
		}
		else {
			std::cout << "Hit wall " << HitTime << " Times" << std::endl;
			HitTime++;
		}

		break;
	}


	playerLookAtX = eye.x + cos(angle) * 10;
	playerLookAtZ = eye.z + sin(angle) * 10;

	// std::cout << eye.x << ", " << eye.y << ", " << eye.z << ", " << angleNumber << std::endl;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	point4  at(playerLookAtX, 0.0, playerLookAtZ, 1.0);
	vec4    up(0.0, 1.0, 0.0, 0.0);

	mat4  mv = LookAt(eye, at, up);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	mat4  p = Perspective(45, (float)WIDTH / HEIGHT, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glBindVertexArray(bufferMaze);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glEnable(GL_LIGHT0);
	glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

	case 'x': leftview *= 1.1; rightview *= 1.1; break;
	case 'X': leftview *= 0.9; rightview *= 0.9; break;
	case 'y': bottom *= 1.1; top *= 1.1; break;
	case 'Y': bottom *= 0.9; top *= 0.9; break;
	case 'z': zNear *= 1.1; zFar *= 1.1; break;
	case 'Z': zNear *= 0.9; zFar *= 0.9; break;
	case 'r': radius *= 2.0; break;
	case 'R': radius *= 0.5; break;
	case 'o': theta += dr; break;
	case 'O': theta -= dr; break;
	case 'p': phi += dr; break;
	case 'P': phi -= dr; break;

	case ' ':  // reset values to their defaults
		leftview = -1.0;
		rightview = 1.0;
		bottom = -1.0;
		top = 1.0;
		zNear = 0.5;
		zFar = 3.0;

		radius = 1.0;
		theta = 0.0;
		phi = 0.0;
		break;
	}

	glutPostRedisplay();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

//按下 上、下、左、右 會觸發的事件
void specialInput(int key, int x, int y)
{
	float step = 1.5;
	switch (key)
	{
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

void start_generate_maze() {
	initGrid();
	drawGrid();
}
int main(int argc, char **argv)
{
	srand((unsigned)time(NULL));
	start_generate_maze();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1366, 768);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("3D Maze");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialInput);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}
