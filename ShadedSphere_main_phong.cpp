/*
 * View.cpp
 *
 *  Created on: Dec 19, 2013
 *      Author: ALEX-2010
 */

#include "Angel.h"
#include "core.h"
#include <vector>
Map map;
int WINDOW_WIDTH = 512;
int WINDOW_HEIGHT = 512;

int direction;
const int NumTimesToSubdivide = 5;
const int NumTriangles = 4096 + 2;  // (4 faces)^(NumTimesToSubdivide + 1)
const int NumVertices = 3 * NumTriangles;

int animispeed = 1;

vec4 points[NumVertices];
vec3 normals[NumVertices];

GLuint model_loc, camera_loc, projection_loc, lightPos_loc;
GLuint vNormal;
GLuint program;
//camera parameters
vec4 cam_eye = vec4(15.0, 10.0, -1, 1.0);
vec4 cam_upvec = vec4(0.0, 1.0, 0.0, 0.0);
vec4 cam_COF = vec4(0.0, 0.0, -1, 1.0);
int cam_move_index = 0;
mat4 cam_moves[4];
GLfloat cam_rotate = 0;
//projection parameters
GLfloat proj_left, proj_right, proj_top, proj_bottom, proj_znear = 1, proj_zfar = 100;

//transformation
mat4 trans_matrix;
GLfloat theta = 0;

vector<GLfloat> dx;
vector<GLfloat> dy;

GLfloat trans;

//light parameters
vec4 light_position(6, 6, 6, 0.0);
vec4 light_ambient(0, 0, 0, 1.0);
vec4 light_diffuse(1.0, 1.0, 1.0, 1.0);
vec4 light_specular(1.0, 1.0, 1.0, 1.0);

//material parameters
vec4 material_ambient(1.0, 0.0, 1.0, 1.0);
vec4 material_diffuse(1.0, 0.8, 0.0, 1.0);
vec4 material_specular(1.0, 0.0, 1.0, 1.0);
GLfloat material_shininess = 200.0;

//ball parameters
GLfloat sphere_width = 2;

int index = 0;
void triangle(const vec4& a, const vec4& b, const vec4& c) {
	vec3 normal = normalize(cross(b - a, c - b));

	normals[index] = normal;
	points[index] = a;
	index++;
	normals[index] = normal;
	points[index] = b;
	index++;
	normals[index] = normal;
	points[index] = c;
	index++;
}

vec4 unit(const vec4& p) {
	float len = p.x * p.x + p.y * p.y + p.z * p.z;

	vec4 t;
	if (len > DivideByZeroTolerance) {
		t = p / sqrt(len);
		t.w = 1.0;
	}

	return t;
}

void divide_triangle(const vec4& a, const vec4& b, const vec4& c, int count) {
	if (count > 0) {
		vec4 v1 = unit(a + b);
		vec4 v2 = unit(a + c);
		vec4 v3 = unit(b + c);
		divide_triangle(a, v1, v2, count - 1);
		divide_triangle(c, v2, v3, count - 1);
		divide_triangle(b, v3, v1, count - 1);
		divide_triangle(v1, v3, v2, count - 1);
	} else {
		triangle(a, b, c);
	}
}

void tetrahedron(int count) {
	vec4 v[4] = { vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 0.942809, -0.333333, 1.0),
			vec4(-0.816497, -0.471405, -0.333333, 1.0), vec4(0.816497,
					-0.471405, -0.333333, 1.0) };

	divide_triangle(v[0], v[1], v[2], count);
	divide_triangle(v[3], v[2], v[1], count);
	divide_triangle(v[0], v[3], v[1], count);
	divide_triangle(v[0], v[2], v[3], count);
}
void step(int);

void animate(int val)
{
	if(trans>=sphere_width)
	{
		glutPostRedisplay();
		trans = 0;
		if(cam_rotate != 0)
		{
			cam_COF = RotateY(cam_rotate) * cam_COF;
			cam_eye = RotateY(cam_rotate) * cam_eye;
			cam_rotate = 0;
		}
		step(1);
	}
	else
	{

		//	std::cout<<trans<<std::endl;
		trans +=0.01;
		cam_COF = cam_moves[cam_move_index] * cam_COF;
		cam_eye = cam_moves[cam_move_index] * cam_eye;

		glutPostRedisplay();
		glutTimerFunc(animispeed, animate, 1);
	}

}
void step(int integer) {
	if (integer != 0) {
		map.move(direction);
		map.update();

	}



	int size = map.snake_old.size();
	int j = map.snake.size()-size;


	list<int>::iterator iter;
	list<int>::iterator iter2;
	iter2 = map.snake.begin();
	if(j>0)
		iter2++;


	dx.clear();
	dy.clear();

	int* oldcord;
	int* newcord;
	for(iter=map.snake_old.begin();iter!=map.snake_old.end();iter++)
	{
		oldcord = map.decrypt(*iter);
		newcord = map.decrypt(*iter2);

//		std::cout<<newcord[0]<<std::endl;
//		std::cout<<newcord[1]<<std::endl;
		dx.push_back(newcord[0]-oldcord[0]);
		dy.push_back(newcord[1]-oldcord[1]);


		iter2++;
	}

	direction = Periodic;
	//map.printArray();


	trans = 0;
	glutPostRedisplay();
	glutTimerFunc(animispeed, animate, 1);

}



void init() {

	cam_moves[0] = Translate(-0.01, 0, 0);
	cam_moves[1] = Translate(0, 0, -0.01);
	cam_moves[2] = Translate(0.01, 0, 0);
	cam_moves[3] = Translate(0, 0, 0.01);
	map.initialize();
	vec4 a(0, 0, 0, 1);
	vec4 b(sphere_width, 0, 0, 1);
	vec4 c(0, 0, sphere_width, 1);
	vec4 d(sphere_width, 0, sphere_width, 1);

	vec3 normal = vec3(0, 1, 0);

	normals[index] = normal;
	points[index] = a;
	index++;
	normals[index] = normal;
	points[index] = b;
	index++;
	normals[index] = normal;
	points[index] = c;
	index++;

	normals[index] = normal;
	points[index] = b;
	index++;
	normals[index] = normal;
	points[index] = c;
	index++;
	normals[index] = normal;
	points[index] = d;
	index++;

	//generate the sphere
	tetrahedron(NumTimesToSubdivide);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer[2];
	glGenBuffers(2, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0) );

	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	vec4 ambient_product = light_ambient * material_ambient;
	vec4 diffuse_product = light_diffuse * material_diffuse;
	vec4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1,
			ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1,
			diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1,
			specular_product);
	lightPos_loc = glGetUniformLocation(program, "LightPosition");
	glUniform4fv(lightPos_loc, 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, mat4(1));
	camera_loc = glGetUniformLocation(program, "Camera");
	projection_loc = glGetUniformLocation(program, "Projection");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	direction = Periodic;
	step(0);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	mat4 camera = LookAt(cam_eye, cam_COF, cam_upvec);
	glUniformMatrix4fv(camera_loc, 1, GL_TRUE, camera);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, mat4(1));

	material_diffuse = vec4(1,1,1,1);
//
//	vec4 ambient_product = light_ambient * material_ambient;
	vec4 diffuse_product = light_diffuse * material_diffuse;
//	vec4 specular_product = light_specular * material_specular;
//
//	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1,
//			ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1,
			diffuse_product);
//	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1,
//			specular_product);

	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	for (int i = -Length / 2; i < Length / 2; i++)
		for (int j = -Width / 2; j < Width / 2; j++) {
			glUniformMatrix4fv(model_loc, 1, GL_TRUE,
					Translate(i * sphere_width, 0, j * sphere_width));
			glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

		}

//	material_diffuse = vec4(0,1,1,1);
//	material_ambient = vec4(0,0,0,1);
//
//		 ambient_product = light_ambient * material_ambient;
//		 diffuse_product = light_diffuse * material_diffuse;
//		 specular_product = light_specular * material_specular;
//
//		glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1,
//				ambient_product);
//		glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1,
//				diffuse_product);
//		glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1,
//				specular_product);
//		glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);




		list<int>::iterator iter;

		int* oldcord;
		int k = 0;
		for(iter=map.snake_old.begin();iter!=map.snake_old.end();iter++)
		{
			oldcord = map.decrypt(*iter);
			int i = (oldcord[0]-Length/2),j = (oldcord[1]-Width/2);

			material_diffuse = vec4(1,0,0,1);
									 diffuse_product = light_diffuse * material_diffuse;
									glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1,
											diffuse_product);
			glUniformMatrix4fv(model_loc, 1, GL_TRUE,
									Translate(i * sphere_width + sphere_width / 2 + (dx[k]*trans),
											sphere_width / 2,
											j * sphere_width + sphere_width / 2 + (dy[k]*trans)));
							glDrawArrays(GL_TRIANGLES, 2 * 3, NumVertices - 6);
					k++;
		}




	for (int i = -Length / 2; i < Length / 2; i++)
		for (int j = -Width / 2; j < Width / 2; j++)
		{
			if(map.isFood(i + Length / 2, j + Width / 2))
			{
				if(map.isSpecial(i + Length / 2, j + Width / 2))
				{
					material_diffuse = vec4(0,0,0,1);
				}
				else
					material_diffuse = vec4(0,1,1,1);
						 diffuse_product = light_diffuse * material_diffuse;
						glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1,
								diffuse_product);
				glUniformMatrix4fv(model_loc, 1, GL_TRUE,
						Translate(i * sphere_width + sphere_width / 2,
								sphere_width / 2,
								j * sphere_width + sphere_width / 2));
				glDrawArrays(GL_TRIANGLES, 2 * 3, NumVertices - 6);

			}
		}

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'd':
		direction = turn_left;
		cam_rotate = -90;
		cam_move_index = (cam_move_index+1)%4;
		break;
	case 'a':
		direction = turn_right;
		cam_rotate = 90;
		cam_move_index = (cam_move_index-1)%4;
		break;
	}
	glutPostRedisplay();
}

void reshape(int width, int height) {

	WINDOW_HEIGHT = height;
	WINDOW_WIDTH = width;

//	mat4 projection = Ortho(proj_left, proj_right, proj_bottom, proj_top,
//			proj_znear, proj_zfar);
	mat4 projection = Perspective(45.0f, 1.0f * WINDOW_WIDTH / WINDOW_HEIGHT,
			proj_znear, proj_zfar);

	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	glViewport(0, 0, width, height);

	glutPostRedisplay();
}

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Sphere");
	glewInit();
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}
