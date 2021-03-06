/*
 *This C program is an exercise in texture mapping
 *user rotation and translation enabled
 *Mipmapping demonstration included
 *SEIS 750
 *Code by Tim Urness
 *Modified by Nathan Gossett
 *Spring 2012
 **/

#include <stdio.h>
#include <GL/Angel.h>
#include <stdlib.h>
#include <math.h>
#include <IL/il.h>

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



//This first texture is our standard black and white checkerboard texture
GLubyte texture[texHeight][texWidth][3];
//and now to make the mipmap transition visible, we're going to manually define all of the other
//mipmap levels.  Normally we wouldn't go through this trouble, but standard mipmapping is pretty invisible
//to the viewer, so we have to take steps to make it visible

GLubyte texture1[32][32][3];
GLubyte texture2[16][16][3];
GLubyte texture3[8][8][3];
GLubyte texture4[4][4][3];
GLubyte texture5[2][2][3];
GLubyte texture6[1][1][3];
//also notice that we have to go all the way down to a 1x1 level, even if that level will never be used


void makeCheckerTexture(void)
{
 int i, j, c;
    
	//our main texture
   for (i = 0; i < texHeight; i++) {
      for (j = 0; j < texWidth; j++) {
         c = (((i/8) + (j/8))%2) * 255;
		 texture[i][j][0] = (GLubyte) c;
         texture[i][j][1] = (GLubyte) c;
         texture[i][j][2] = (GLubyte) c;
      }
   }

   //and now the rest of our levels
  for (i = 0; i < 32; i++) {
      for (j = 0; j < 32; j++) {
         c = (((i/4) + (j/4))%2) * 255;
		 texture1[i][j][0] = (GLubyte) c; //red
         texture1[i][j][1] = 0;
         texture1[i][j][2] = 0;
      }
   }

   for (i = 0; i < 16; i++) {
      for (j = 0; j < 16; j++) {
         c = (((i/2) + (j/2))%2) * 255;
		 texture2[i][j][0] = 0; //green
         texture2[i][j][1] = (GLubyte) c;
         texture2[i][j][2] = 0;
      }
   }

   for (i = 0; i < 8; i++) {
      for (j = 0; j < 8; j++) {
         c = (((i) + (j))%2) * 255;
		 texture3[i][j][0] = 0; //blue
         texture3[i][j][1] = 0;
         texture3[i][j][2] = (GLubyte) c;
      }
   }

   for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
         c = (((i) + (j))%2) * 255;
		 texture4[i][j][0] = (GLubyte) c; //red
         texture4[i][j][1] = 0;
         texture4[i][j][2] = 0;
      }
   }

   for (i = 0; i < 2; i++) {
      for (j = 0; j < 2; j++) {
         c = (((i) + (j))%2) * 255;
		texture2[i][j][0] = 0; //green
         texture2[i][j][1] = (GLubyte) c;
         texture2[i][j][2] = 0;
      }
   }

   for (i = 0; i < 1; i++) {
      for (j = 0; j < 1; j++) {
         c = (((i) + (j))%2) * 255;
		 texture3[i][j][0] = 0; //blue
         texture3[i][j][1] = 0;
         texture3[i][j][2] = (GLubyte) c;
      }
   }
}


void init(void)
{    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glEnable(GL_DEPTH_TEST);

   vec4 squareverts[6];
   vec2 texcoords[6];
   squareverts[0] = vec4(-1, -1, 0, 1);
   texcoords[0] = vec2(0, 0);
   squareverts[1] = vec4(1, -1, 0, 1);
   texcoords[1] = vec2(1, 0);
   squareverts[2] = vec4(1, 1, 0, 1);
   texcoords[2] = vec2(1, 1);
   squareverts[3] = vec4(1, 1, 0, 1);
   texcoords[3] = vec2(1, 1);
   squareverts[4] = vec4(-1, 1, 0, 1);
   texcoords[4] = vec2(0, 1);
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

   makeCheckerTexture();
 

   glGenTextures(1, texName);
   glBindTexture(GL_TEXTURE_2D, texName[0]);

   //Note we now have mipmapping filters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

   //define the 0 level of our texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 
                0, GL_RGB, GL_UNSIGNED_BYTE, texture);

   //***************************************
   //UNCOMMENT ONE OF THE FOLLOWING OPTIONS
   //***************************************

    //OPTION 1: Manually define all mipmap levels
      //usually we wouldn't go through this trouble, but we want to see the transition
     //Note that each level is marked with which level it is (0 is the largest size)
     //and each level has a width and height that is half of the level above it
      glTexImage2D(GL_TEXTURE_2D, 1, GL_RGB, 32, 32, 
                0, GL_RGB, GL_UNSIGNED_BYTE, texture1);
	  glTexImage2D(GL_TEXTURE_2D, 2, GL_RGB, 16, 16, 
                0, GL_RGB, GL_UNSIGNED_BYTE, texture2);
	  glTexImage2D(GL_TEXTURE_2D, 3, GL_RGB, 8, 8, 
                0, GL_RGB, GL_UNSIGNED_BYTE, texture3);
	  glTexImage2D(GL_TEXTURE_2D, 4, GL_RGB, 4, 4, 
                0, GL_RGB, GL_UNSIGNED_BYTE, texture4);
	  glTexImage2D(GL_TEXTURE_2D, 5, GL_RGB, 2, 2, 
                0, GL_RGB, GL_UNSIGNED_BYTE, texture5);
	  glTexImage2D(GL_TEXTURE_2D, 6, GL_RGB, 1, 1, 
                0, GL_RGB, GL_UNSIGNED_BYTE, texture6);

    //OPTION 2: Let OpenGL generate all mipmap levels for us
	  //this is what we normally do, and you should have seamless transitions
	  //This one line of code takes the place of all of the other code we had to do 
	  //manually
//	  glGenerateMipmap(GL_TEXTURE_2D);

	  

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");
	
	texMap = glGetUniformLocation(program, "texture");
	glUniform1i(texMap, 0);//assign this one to texture unit 0


	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	texCoord = glGetAttribLocation(program, "texCoord");
	glEnableVertexAttribArray(texCoord);
	glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	

	mv =  LookAt(vec4(0,0,5.0+z_distance,1),vec4(0,0,0,1),vec4(0,1,0,0));
	mv = mv * RotateX(view_rotx) * RotateY(view_roty) * RotateZ(view_rotz);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glActiveTexture(GL_TEXTURE0);
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
      default:
         break;
   }
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