/*
 *This C program is an exercise in texture mapping
 *user rotation and translation enabled
 *SEIS750
 *Nathan Gossett
 *Spring 2012
 **/

#include <stdio.h>
#include <GL/Angel.h>
#include <stdlib.h>
#include <math.h>

#pragma comment(lib, "glew32.lib")


#define WIDTH 500
#define HEIGHT 400
#define TRUE 1
#define FALSE 0


int right_button_down = FALSE;
int left_button_down = FALSE;

int prevMouseX;
int prevMouseY;

double view_rotx = 0.0;
double view_roty = 0.0;
double view_rotz = 0.0;
double z_distance;

/*	Create checkerboard texture	*/
#define	texWidth 64
#define	texHeight 64

GLuint vao[1];
GLuint vbo[3];
GLuint texName[1];

//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;
GLuint vPosition;
GLuint texCoord;
GLuint texMap;

GLuint program;


GLubyte texture[texHeight][texWidth][3];

int magFilter = GL_NEAREST;
int minFilter = GL_NEAREST;
int sWrap = GL_REPEAT;
int tWrap = GL_REPEAT;

void makeCheckerTexture(void)
{
 int i, j, c;
    
   for (i = 0; i < texHeight; i++) {
      for (j = 0; j < texWidth; j++) {
         c = (((i/8) + (j/8))%2) * 255;
		 texture[i][j][0] = (GLubyte) c;
         texture[i][j][1] = (GLubyte) c;
         texture[i][j][2] = (GLubyte) c;
      }
   }
}

void makeCircleTexture(){
	//if you're bored, try to figure out how you would make a polkadot texture

}


void init(void)
{    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glEnable(GL_DEPTH_TEST);

   //create a vao for a simple square
   vec4 squareverts[6];
   vec2 texcoords[6];
   squareverts[0] = vec4(-1, -1, 0, 1);
   texcoords[0] = vec2(0, 0);
   squareverts[1] = vec4(1, -1, 0, 1);
   texcoords[1] = vec2(1.5, 0);
   squareverts[2] = vec4(1, 1, 0, 1);
   texcoords[2] = vec2(1.5, 1.5);
   squareverts[3] = vec4(1, 1, 0, 1);
   texcoords[3] = vec2(1.5, 1.5);
   squareverts[4] = vec4(-1, 1, 0, 1);
   texcoords[4] = vec2(0, 1.5);
   squareverts[5] = vec4(-1, -1, 0, 1);
   texcoords[5] = vec2(0, 0);

   program = InitShader( "vshader-texture.glsl", "fshader-texture.glsl" );

	// Create a vertex array object
    glGenVertexArrays( 1, &vao[0] );

    // Create and initialize any buffer objects
	glBindVertexArray( vao[0] );
	glGenBuffers( 2, &vbo[0] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(squareverts), squareverts, GL_STATIC_DRAW);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	texCoord = glGetAttribLocation(program, "texCoord");
	glEnableVertexAttribArray(texCoord);
	glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);


	//populate main memory with a texture
	makeCheckerTexture();
 
	//create an appropriate number of texture objects
	glGenTextures(1, texName);
	//make sure you're bound to the correct texture object
	glBindTexture(GL_TEXTURE_2D, texName[0]);
	
	//Now move the texture data from main memory to texture memory 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

	//You'll need a uniform sampler in your fragment shader to get at the texels
	texMap = glGetUniformLocation(program, "texture");

	//assign this one to texture unit 0
	glUniform1i(texMap, 0);
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);

	mv =  LookAt(vec4(0,0,5.0+z_distance,1),vec4(0,0,0,1),vec4(0,1,0,0));
	mv = mv * RotateX(view_rotx) * RotateY(view_roty) * RotateZ(view_rotz);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	//Make sure you activate the right texture unit
	glActiveTexture(GL_TEXTURE0);
	
	//Make sure  you bind to the correct texture
	glBindTexture(GL_TEXTURE_2D, texName[0]);
	glDrawArrays( GL_TRIANGLES, 0, 6 );

   glutSwapBuffers();


}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);

	 p = Perspective(60.0, (float)w/(float)h, 1.0, 30.0);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
}

void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
	  case 'a':
		  if (magFilter == GL_NEAREST)
		  {
			  magFilter = GL_LINEAR;
		  }
		  else
		  {
			  magFilter = GL_NEAREST;
		  }
		  break;
	  case 'i':
		  if (minFilter == GL_NEAREST)
		  {
			  minFilter = GL_LINEAR;
		  }
		  else
		  {
			  minFilter = GL_NEAREST;
		  }
		  break;
	  case 's':
		  if (sWrap == GL_REPEAT)
		  {
			  sWrap = GL_CLAMP;
		  }
		  else
		  {
			  sWrap = GL_REPEAT;
		  }
		  break;
	  case 't':
		  if (tWrap == GL_REPEAT)
		  {
			  tWrap = GL_CLAMP;
		  }
		  else
		  {
			  tWrap = GL_REPEAT;
		  }
		  break;
      default:
         break;
   }
   glutPostRedisplay();
}

void mouse_dragged(int x, int y) {
	double thetaY, thetaX;
	if (left_button_down) {
		thetaY = 360.0 *(x-prevMouseX)/WIDTH;    
		thetaX = 360.0 *(prevMouseY - y)/HEIGHT;
		prevMouseX = x;
		prevMouseY = y;
		view_rotx += thetaX;
		view_roty += thetaY;
	}
	else if (right_button_down) {
		z_distance = 5.0*(prevMouseY-y)/HEIGHT;
	}
  glutPostRedisplay();
}


void mouse(int button, int state, int x, int y) {
  //establish point of reference for dragging mouse in window
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      left_button_down = TRUE;
	  prevMouseX= x;
      prevMouseY = y;
    }

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
      right_button_down = TRUE;
      prevMouseX = x;
      prevMouseY = y;
    }
    else if (state == GLUT_UP) {
      left_button_down = FALSE;
	  right_button_down = FALSE;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Texturing");
	glewExperimental = GL_TRUE;

	glewInit();
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_dragged);    
	glutMainLoop();
	return 0; 
}