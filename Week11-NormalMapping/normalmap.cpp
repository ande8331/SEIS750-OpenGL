/*
 *This C program is an exercise in normal mapping
 *user rotation and translation enabled
 *Nathan Gossett
 *Spring 2012
 **/

#include <stdio.h>
#include <GL/Angel.h>
#include <stdlib.h>
#include <math.h>
#include <IL/il.h>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib,"ILUT.lib")
#pragma comment(lib,"DevIL.lib")
#pragma comment(lib,"ILU.lib")


#define WIDTH 500
#define HEIGHT 400
#define TRUE 1
#define FALSE 0


int right_button_down = FALSE;
int left_button_down = FALSE;

int prevMouseX;
int prevMouseY;

double view_rotx = 180.0;
double view_roty = 0.0;
double view_rotz = 0.0;
double z_distance = -2;

static GLuint texName[3];


GLuint vao[1];
GLuint vbo[4];

//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;
GLuint vPosition;
GLuint vNormal;
GLuint vTangent;
GLuint texCoord;
GLuint texMap;
GLuint normalMap;

GLuint vAmbientDiffuseColor;
GLuint vSpecularColor;
GLuint vSpecularExponent;
GLuint light_position;
GLuint light_color;
GLuint ambient_light;


GLuint program;


ILuint loadTexFile(const char* filename){
	
	ILboolean success; /* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
    it can have different value (because it's just typedef of unsigned char), but this sould be
    avoided.
    Variable success will be used to determine if some function returned success or failure. */


	/* Before calling ilInit() version should be checked. */
	  if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	  {
		/* wrong DevIL version */
		printf("Wrong IL version");
		exit(1);
	  }
 
	  
	  success = ilLoadImage(filename); /* Loading of image "image.jpg" */
	if (success){ /* If no error occured: */
		  if(ilGetInteger(IL_IMAGE_BPP) == 3){
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every colour component into
		  unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
		  }else if(ilGetInteger(IL_IMAGE_BPP) == 4){
			  success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		  }else{
			  success = false;
		  }
		if (!success){
		  /* Error occured */
		 printf("failed conversion to unsigned byte");
		 exit(1);
		}
	}else{
		/* Error occured */
	   printf("Failed to load image ");
	   printf(filename);
		exit(1);
	}
}


void init(void)
{    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glEnable(GL_DEPTH_TEST);

	//we have a simple two-triangle square
   vec4 squareverts[6]; //vertex locations
   vec2 texcoords[6]; //texture coordinates
   vec4 squarenormals[6]; //unmodified normal vectors
   vec4 squaretangents[6]; //tangent directions
   squareverts[0] = vec4(-1, -1, 0, 1);
   texcoords[0] = vec2(0, 0);
   squarenormals[0] = vec4(0, 0, 1, 0);
   squaretangents[0] = vec4(1, 0, 0, 0);
   squareverts[1] = vec4(1, -1, 0, 1);
   texcoords[1] = vec2(1, 0);
   squarenormals[1] = vec4(0, 0, 1, 0);
   squaretangents[1] = vec4(1, 0, 0, 0);
   squareverts[2] = vec4(1, 1, 0, 1);
   texcoords[2] = vec2(1, 1);
   squarenormals[2] = vec4(0, 0, 1, 0);
   squaretangents[2] = vec4(1, 0, 0, 0);
   squareverts[3] = vec4(1, 1, 0, 1);
   texcoords[3] = vec2(1, 1);
   squarenormals[3] = vec4(0, 0, 1, 0);
   squaretangents[3] = vec4(1, 0, 0, 0);
   squareverts[4] = vec4(-1, 1, 0, 1);
   texcoords[4] = vec2(0, 1);
   squarenormals[4] = vec4(0, 0, 1, 0);
   squaretangents[4] = vec4(1, 0, 0, 0);
   squareverts[5] = vec4(-1, -1, 0, 1);
   texcoords[5] = vec2(0, 0);
   squarenormals[5] = vec4(0, 0, 1, 0);
   squaretangents[5] = vec4(1, 0, 0, 0);

   program = InitShader( "vshader-normal.glsl", "fshader-normal.glsl" );

	// Create a vertex array object
    glGenVertexArrays( 1, &vao[0] );

    // Create and initialize any buffer objects
	glBindVertexArray( vao[0] );
	glGenBuffers( 4, &vbo[0] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(squareverts), squareverts, GL_STATIC_DRAW);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[2] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(squarenormals), squarenormals, GL_STATIC_DRAW);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[3] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(squaretangents), squaretangents, GL_STATIC_DRAW);

	ILuint ilTexID[3]; /* ILuint is a 32bit unsigned integer.
    Variable texid will be used to store image name. */

	ilInit(); /* Initialization of OpenIL */
	ilGenImages(3, ilTexID); /* Generation of three image names for OpenIL image loading */
	glGenTextures(3, texName); //and we eventually want the data in an OpenGL texture
 


	ilBindImage(ilTexID[0]); /* Binding of IL image name */
	loadTexFile("images/brickwork-texture.jpg");
	glBindTexture(GL_TEXTURE_2D, texName[0]); //bind OpenGL texture name

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
   glGenerateMipmap(GL_TEXTURE_2D);

    ilBindImage(ilTexID[1]);
	glBindTexture(GL_TEXTURE_2D, texName[1]);
   loadTexFile("images/brickwork_normal-map.jpg");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
   glGenerateMipmap(GL_TEXTURE_2D);


   //This third texture is a completely flat normal map for comparison
	ilBindImage(ilTexID[2]);
	glBindTexture(GL_TEXTURE_2D, texName[2]);
    loadTexFile("images/flat.png");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());

   
    ilDeleteImages(3, ilTexID); //we're done with OpenIL, so free up the memory


	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");
	vAmbientDiffuseColor = glGetAttribLocation(program, "vAmbientDiffuseColor");
	vSpecularColor = glGetAttribLocation(program, "vSpecularColor");
	vSpecularExponent = glGetAttribLocation(program, "vSpecularExponent");
	light_position = glGetUniformLocation(program, "light_position");
	light_color = glGetUniformLocation(program, "light_color");
	ambient_light = glGetUniformLocation(program, "ambient_light");
	
	texMap = glGetUniformLocation(program, "texture");
	glUniform1i(texMap, 0);//assign color texture to texture unit 0
	normalMap = glGetUniformLocation(program, "normalMap");
	glUniform1i(normalMap, 1);//assign normal map to texture unit 1


	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	texCoord = glGetAttribLocation(program, "texCoord");
	glEnableVertexAttribArray(texCoord);
	glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[2] );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[3] );
	vTangent = glGetAttribLocation(program, "vTangent");
	glEnableVertexAttribArray(vTangent);
	glVertexAttribPointer(vTangent, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	
    mat4 camera = mv =  LookAt(vec4(0,0,5.0+z_distance,1),vec4(0,0,0,1),vec4(0,1,0,0));

	mv = mv*Translate(1.5, 0, 0)* RotateX(view_rotx) * RotateY(view_roty) * RotateZ(view_rotz);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	//If you want a square with just the lighting and no color texture, you'll need these
	glVertexAttrib4fv(vAmbientDiffuseColor, vec4(.7, .7, .7, 1));
	glVertexAttrib4fv(vSpecularColor, vec4(.7f,.7f,.7f,1.0f));
	glVertexAttrib1f(vSpecularExponent, 30);
	glUniform4fv(light_position, 1, mv*vec4(10, 10, 50, 1));
	glUniform4fv(light_color, 1, vec4(1,1,1,1));
	glUniform4fv(ambient_light, 1, vec4(.1, .1, .1, 1));

	//Texture unit 0 handles color texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texName[0]); //which texture do we want?
	//Texture unit 1 handles normal map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texName[1]);
	glDrawArrays( GL_TRIANGLES, 0, 6 );

	//Just for comparison, lets also draw another square to the side with a flat normal map
	mv = camera*Translate(-1.5, 0, 0)* RotateX(view_rotx) * RotateY(view_roty) * RotateZ(view_rotz);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texName[2]);
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
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowSize(WIDTH, HEIGHT);
   glutInitWindowPosition(100, 100);
   glutCreateWindow("Normal Mapping");
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