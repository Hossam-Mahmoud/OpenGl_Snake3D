/*
 * View.cpp
 *
 *  Created on: Dec 19, 2013
 *      Author: ALEX-2010
 */

#include "Angel.h"

int WINDOW_WIDTH = 512;
int WINDOW_HEIGHT = 512;



const int NumTimesToSubdivide = 3;//5;
const int NumTriangles        = 256;//4096;  // (4 faces)^(NumTimesToSubdivide + 1)
const int NumVertices         = 3 * NumTriangles;

vec4 points[NumVertices];
vec3 normals[NumVertices];

GLuint  model_loc, camera_loc, projection_loc;
GLuint vNormal;

void init()
{

}



void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glutSwapBuffers();
}


void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {

    }
}

void
reshape( int width, int height )
{
    /*glViewport( 0, 0, width, height );

    GLfloat left = -2.0, right = 2.0;
    GLfloat top = 2.0, bottom = -2.0;
    GLfloat zNear = -20.0, zFar = 20.0;

    GLfloat aspect = GLfloat(width)/height;

    if ( aspect > 1.0 ) {
        left *= aspect;
        right *= aspect;
    }
    else {
        top /= aspect;
        bottom /= aspect;
    }

    mat4 projection = Ortho( left, right, bottom, top, zNear, zFar );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );*/
}




int main(int argc, char **argv) {

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH );
	glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT );
	glutCreateWindow( "Sphere" );
	glewInit();
	init();

	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutReshapeFunc( reshape );

	glutMainLoop();


	return 0;
}
