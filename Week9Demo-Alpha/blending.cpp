/*
 *
 *Blending example
 *SEIS750
 *Fall 2012
 **/

#include <stdio.h>
#include <GL/Angel.h>
#include <stdlib.h>
#include <math.h>

#pragma comment(lib, "glew32.lib")
//store window width and height
int ww=500, wh=500;

GLuint vao[1];
GLuint vbo[1];

//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;
GLuint vPosition;
GLuint vColor;

GLuint program;

void display(void)
{
	glEnable(GL_ALPHA);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*clear all pixels*/
	glClear(GL_COLOR_BUFFER_BIT );

	mv =  LookAt(vec4(0,0,5.0,1),vec4(0,0,0,1),vec4(0,1,0,0));
	
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
  
	//draw stuff here

	glVertexAttrib4fv(vColor, vec4(1, 0, 0, 0.4));
	glDrawArrays(GL_TRIANGLES, 0, 6);

	mv = mv*Translate(0.4, 0.4, 0);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	glVertexAttrib4fv(vColor, vec4(0, 1, 0, 0.5));
	glDrawArrays(GL_TRIANGLES, 0, 6);
		
    glFlush();
  glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y) {
	/*exit when the escape key is pressed*/
	if (key == 27)
		exit(0);
	glutPostRedisplay();
}

void init(){
	program = InitShader( "vshader-blend.glsl", "fshader-blend.glsl" );


   vec4 squareverts[6];
   squareverts[0] = vec4(-1, -1, 0, 1);
   squareverts[1] = vec4(1, -1, 0, 1);
   squareverts[2] = vec4(1, 1, 0, 1);
   squareverts[3] = vec4(1, 1, 0, 1);
   squareverts[4] = vec4(-1, 1, 0, 1);
   squareverts[5] = vec4(-1, -1, 0, 1);

	// Create a vertex array object
    glGenVertexArrays( 1, &vao[0] );

    // Create and initialize any buffer objects
	glBindVertexArray( vao[0] );
	glGenBuffers( 1, &vbo[0] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(squareverts), squareverts, GL_STATIC_DRAW);
	 
 

	//glLinkProgram( prog);
	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");
	


	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	vColor = glGetAttribLocation(program, "vColor");



}

void reshape(int width, int height){
	 p = Perspective(60.0, (float)width/(float)height, 1.0, 30.0);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glViewport( 0, 0, width, height );
}

int main(int argc, char **argv)
{
  /*set up window for display*/
  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0); 
  glutInitWindowSize(ww, wh);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutCreateWindow("Blending");  
  glewExperimental = GL_TRUE;

  glewInit();
  init();

  glutDisplayFunc(display);
  glutKeyboardFunc(Keyboard);
  glutReshapeFunc(reshape);

  glutMainLoop();
  return 0;
}