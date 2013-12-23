/*

 * View.cpp
 *
 *  Created on: Dec 19, 2013
 *      Author: ALEX-2010


#include "Angel.h"

int WINDOW_WIDTH = 512;
int WINDOW_HEIGHT = 512;



const int NumTimesToSubdivide = 5;
const int NumTriangles        = 4096;  // (4 faces)^(NumTimesToSubdivide + 1)
const int NumVertices         = 3 * NumTriangles;

vec4 points[NumVertices];
vec3 normals[NumVertices];

GLuint  model_loc, camera_loc, projection_loc, lightPos_loc;
GLuint vNormal;


//camera parameters
vec4 cam_eye;
vec4 cam_upvec;
vec4 cam_COF;

//projection parameters
GLfloat proj_left, proj_right,
		proj_top, proj_bottom,
		proj_znear, proj_zfar;

//transformation
mat4 trans_matrix;
GLfloat theta=0;

//light parameters
vec4 light_position( 0.0, 0.0, 2.0, 0.0 );
vec4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
vec4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
vec4 light_specular( 1.0, 1.0, 1.0, 1.0 );

//material parameters
vec4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
vec4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
vec4 material_specular( 1.0, 0.0, 1.0, 1.0 );
GLfloat  material_shininess = 200.0;

//ball parameters
GLfloat ball_width =2;



int index =0;
void triangle( const vec4& a, const vec4& b, const vec4& c )
{
    vec3  normal = normalize( cross(b - a, c - b) );

    normals[index] = normal;  points[index] = a;  index++;
    normals[index] = normal;  points[index] = b;  index++;
    normals[index] = normal;  points[index] = c;  index++;
}

vec4 unit( const vec4& p )
{
    float len = p.x*p.x + p.y*p.y + p.z*p.z;

    vec4 t;
    if ( len > DivideByZeroTolerance ) {
        t = p / sqrt(len);
        t.w = 1.0;
    }

    return t;
}

void divide_triangle( const vec4& a, const vec4& b, const vec4& c, int count )
{
    if ( count > 0 ) {
        vec4 v1 = unit( a + b );
        vec4 v2 = unit( a + c );
        vec4 v3 = unit( b + c );
        divide_triangle(  a, v1, v2, count - 1 );
        divide_triangle(  c, v2, v3, count - 1 );
        divide_triangle(  b, v3, v1, count - 1 );
        divide_triangle( v1, v3, v2, count - 1 );
    }
    else {
        triangle( a, b, c );
    }
}

void tetrahedron( int count )
{
    vec4 v[4] = {
        vec4( 0.0, 0.0, 1.0, 1.0 ),
        vec4( 0.0, 0.942809, -0.333333, 1.0 ),
        vec4( -0.816497, -0.471405, -0.333333, 1.0 ),
        vec4( 0.816497, -0.471405, -0.333333, 1.0 )
    };

    divide_triangle( v[0], v[1], v[2], count );
    divide_triangle( v[3], v[2], v[1], count );
    divide_triangle( v[0], v[3], v[1], count );
    divide_triangle( v[0], v[2], v[3], count );
}

void init()
{
	//generate the sphere
	tetrahedron( NumTimesToSubdivide );

	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	 GLuint buffer;
	 glGenBuffers( 1, &buffer );
	 glBindBuffer( GL_ARRAY_BUFFER, buffer );
	 glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), NULL, GL_STATIC_DRAW );
	 glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
	 glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals );

	 GLuint program = InitShader( "src/vshader.glsl", "src/fshader.glsl" );
	 glUseProgram( program );

	 GLuint vPosition = glGetAttribLocation( program, "vPosition" );
	 glEnableVertexAttribArray( vPosition );
	 glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	 GLuint vNormal = glGetAttribLocation( program, "vNormal" );
	 glEnableVertexAttribArray( vNormal );
	 glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), BUFFER_OFFSET(0) );

	 vec4 ambient_product = light_ambient * material_ambient;
	 vec4 diffuse_product = light_diffuse * material_diffuse;
	 vec4 specular_product = light_specular * material_specular;

	 glUniform4fv( glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product );
	 glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product );
	 glUniform4fv( glGetUniformLocation(program, "SpecularProduct"), 1, specular_product );
	 lightPos_loc = glGetUniformLocation(program, "LightPosition");
	 glUniform4fv( lightPos_loc , 1, light_position );
	 glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );

	 model_loc = glGetUniformLocation( program, "Model" );
	 glUniformMatrix4fv(model_loc, 1,  GL_TRUE,  mat4(1));
	 camera_loc = glGetUniformLocation( program, "Camera" );
	 projection_loc = glGetUniformLocation( program, "Projection" );

	 glEnable( GL_DEPTH_TEST );
	 glClearColor( 1.0, 1.0, 1.0, 1.0 );


}



void
display( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	cam_COF = vec4( 0.0, 0.0, 0.0, 1.0 );
	cam_eye = vec4(2.0, 2.0, 2.0, 1.0 );
	cam_upvec =  vec4( 0.0, 1.0, 0.0, 0.0 );

    mat4 camera = LookAt( cam_eye, cam_COF, cam_upvec);
    glUniformMatrix4fv( camera_loc, 1, GL_TRUE, camera );

    glUniformMatrix4fv(model_loc, 1, GL_TRUE, mat4(1));

    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

    glUniformMatrix4fv(model_loc, 1, GL_TRUE, Translate(0,0,ball_width));

    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

    glUniformMatrix4fv(model_loc, 1, GL_TRUE, Translate(0,0,ball_width*2));
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

    glUniformMatrix4fv(model_loc, 1, GL_TRUE, Translate(0,0,ball_width*3));
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

    glUniformMatrix4fv(model_loc, 1, GL_TRUE, Translate(0,0,-ball_width));
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

    glUniformMatrix4fv(model_loc, 1, GL_TRUE, Translate(-ball_width,0,0));
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

    glUniformMatrix4fv(model_loc, 1, GL_TRUE, Translate(ball_width,0,0));
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

    glutSwapBuffers();
}


void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
    case 'i':
    	theta += 5;
    	mat4 model = RotateY(theta);
    	//glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
    	glUniform4fv( lightPos_loc, 1, model*light_position );
    	break;
    }

    glutPostRedisplay();
}

void
reshape( int width, int height )
{

	WINDOW_HEIGHT = height;
	WINDOW_WIDTH = width;
    proj_left = -5.0; proj_right = 5.0;
    proj_top = 5.0; proj_bottom = -5.0;
    proj_znear= -20.0, proj_zfar = 20.0;

    GLfloat aspect = GLfloat(width)/height;

    if ( aspect > 1.0 ) {
        proj_left *= aspect;
        proj_right *= aspect;
    }
    else {
        proj_top /= aspect;
        proj_bottom /= aspect;
    }

    mat4 projection = Ortho( proj_left, proj_right, proj_bottom, proj_top, proj_znear, proj_zfar );
    glUniformMatrix4fv( projection_loc, 1, GL_TRUE, projection );

    glViewport( 0, 0, width, height );

    glutPostRedisplay();
}




int main(int argc, char **argv) {

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_MULTISAMPLE | GLUT_DEPTH );
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
*/
