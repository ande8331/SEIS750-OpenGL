/*
 *Skeleton transform program
 *COS490
 *Fall 2011
 **/

#include <GL/Angel.h>
#include <math.h>
#pragma comment(lib, "glew32.lib")

//store window width and height
int ww=500, wh=500;

#define M_PI 3.14159265358979323846



GLuint vao[1];
GLuint vbo[2];

//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;
GLuint vPosition;
GLuint vColor;

GLdouble tx = 0.0;
GLdouble ty = 0.0;
GLdouble tz = 0.0;

GLdouble rx = 0.0;
GLdouble ry = 0.0;
GLdouble rz = 0.0;

GLdouble sx = 1.0;
GLdouble sy = 1.0;
GLdouble sz = 1.0;

vec4 cubeVerts[36];
vec4 cubeColors[36];

void generateCube(){
	for(int i=0; i<6; i++){
		cubeColors[i] = vec4(0.0, 1.0, 1.0, 1.0); //front
	}
	cubeVerts[0] = vec4(1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[1] = vec4(1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[2] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[3] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[4] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[5] = vec4(1.0f, -1.0f, 1.0f, 1.0);
	

	for(int i=6; i<12; i++){
		cubeColors[i] = vec4(1.0, 0.0, 1.0, 1.0); //back
	}
	cubeVerts[6] = vec4(-1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[7] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[8] = vec4(1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[9] = vec4(1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[10] = vec4(1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[11] = vec4(-1.0f, -1.0f, -1.0f, 1.0);

	for(int i=12; i<18; i++){
		cubeColors[i] = vec4(1.0, 1.0, 0.0, 1.0); //left
	}
	cubeVerts[12] = vec4(1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[13] = vec4(1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[14] = vec4(1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[15] = vec4(1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[16] = vec4(1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[17] = vec4(1.0f, 1.0f, 1.0f, 1.0);
	
	for(int i=18; i<24; i++){
		cubeColors[i] = vec4(1.0, 0.0, 0.0, 1.0); //right
	}
	cubeVerts[18] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[19] = vec4(-1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[20] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[21] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[22] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[23] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
	
	for(int i=24; i<30; i++){
		cubeColors[i] = vec4(0.0, 0.0, 1.0, 1.0); //top
	}
	cubeVerts[24] = vec4(1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[25] = vec4(1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[26] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[27] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[28] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[29] = vec4(1.0f, 1.0f, 1.0f, 1.0);
	
	for(int i=30; i<36; i++){
		cubeColors[i] = vec4(0.0, 1.0, 0.0, 1.0); //bottom
	}
	cubeVerts[30] = vec4(1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[31] = vec4(1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[32] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[33] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[34] = vec4(-1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[35] = vec4(1.0f, -1.0f, -1.0f, 1.0);
}

void display(void)
{
  /*clear all pixels*/
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	// we'll explain this later, but it's setting our default modelview matrix
    mv = LookAt(vec4(0, 0, 20, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));

	// any additional modelview transformations would happen here
	
	// now that our modelview matrix is all set to go, we send it to our shaders
	// modelview is the same for all vertices in an object, so it's a uniform rather 
	// than an attribute

	/* Alternate method
	mat4 trans = mat4(1.0);
	trans[0][3] = tx;
	trans[1][3] = ty;
	trans[2][3] = tz;
	mv = mv*trans;
	*/

	/* 180 degree rotation matrix of point [1 2 1 1]
	mat4 trans = mat4(1.0);
	trans[0][0] = cos(180);
	trans[1][0] = sin(180);
	trans[0][1] = -sin(180);
	trans[1][1] = cos(180);

	[-1]   [-1 0 0 0]      [cos(180) sin(180) 0 0]
	[-2]   [ 0 -1 0 0]     [-sin(180) cos(180) 0 0]
	[1]  = [0 0 1 0]    =  [0 0 1 0]
	[1]    [0 0 0 1]       [0 0 0 1]
	*/

	mv = mv*Translate(tx, ty, tz);
	/*
	mat4 trans = mat4(1.0);
	trans[0][0] = cos(rx);
	trans[1][0] = sin(ry);
	trans[0][1] = -sin(rx);
	trans[1][1] = cos(ry);
	*/
	ry += 1.0;
	mv = mv*RotateX(rx);
	mv = mv*RotateY(ry);
	mv = mv*RotateZ(rz);

	mv = mv*Scale(sx, sy, sz);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	
	// and we also need to send our projection matrix, which again is more appropriately
	// a uniform instead of an attribute since it's the same for every vertex
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
    
	// Now we have a vertex array that has all of our cube vertex locations and colors
	glBindVertexArray( vao[0] );
	glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the cube 


	 mv = LookAt(vec4(0, 0, 20, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));
	 
	 static float rx = 0.0;
	 static float ry = 0.0;
	 static float rz = 0.0;

	 	 
	 mv = mv * RotateY(ry);
	 mv = mv*Translate(4, 0, 0);
	 mv = mv * RotateY(-ry);
	 rx -= 1.0;
	 ry -= 1.0;
	 rz -= 1.0;

	 mv = mv * RotateX(rx);
	 //mv = mv * RotateZ(rz);

	 glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	 glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the cube 

  /*start processing buffered OpenGL routines*/
  glutSwapBuffers();
}

/* Function call for game reset */
void my_special(int key, int x, int y) 
{
    if (key == GLUT_KEY_UP)
    {
		ty += 1.0;
    }

	if (key == GLUT_KEY_DOWN)
	{
		ty -= 1.0;
	}

	if (key == GLUT_KEY_RIGHT)
	{
		tx += 1.0;
	}

	if (key == GLUT_KEY_LEFT)
	{
		tx -= 1.0;
	}
		glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
	/*exit when the escape key is pressed*/
	if (key == 27)
		exit(0);

	if (key == 'a')
	{
		tz -= 1.0;
	}

	if (key == 'z')
	{
		tz += 1.0;
	}

	if (key == 'q')
	{
		rx += 1.0;
	}

	if (key == 'w')
	{
		rx -= 1.0;
	}

	if (key == 'e')
	{
		ry += 1.0;
	}

	if (key == 'r')
	{
		ry -= 1.0;
	}

	if (key == 't')
	{
		rz += 1.0;
	}

	if (key == 'y')
	{
		rz -= 1.0;
	}

	if (key == 'g')
	{
		sx += 1.0;
	}

	if (key == 'h')
	{
		sx -= 1.0;
	}

	if (key == 'j')
	{
		sy += 1.0;
	}

	if (key == 'k')
	{
		sy -= 1.0;
	}

	if (key == 'l')
	{
		sz += 1.0;
	}

	if (key == ';')
	{
		sz -= 1.0;
	}
	glutPostRedisplay();
}

void init() {

  /*select clearing (background) color*/
  glClearColor(0.0, 0.0, 0.0, 0.0);


  //populate our arrays
  generateCube();


   // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader-transform.glsl", "fshader-transform.glsl" );
    glUseProgram( program );

	// Create a vertex array object
    glGenVertexArrays( 1, &vao[0] );

    // Create and initialize any buffer objects
	glBindVertexArray( vao[0] );
	glGenBuffers( 2, &vbo[0] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
	// notice that since position is unique for every vertex, we treat it as an 
	// attribute instead of a uniform
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//grab pointers for our modelview and perspecive uniform matrices
	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

  //Only draw the things in the front layer
	glEnable(GL_DEPTH_TEST);
}


void reshape(int width, int height){
	ww= width;
	wh = height;
	//field of view angle, aspect ratio, closest distance from camera to object, largest distanec from camera to object
	p = Perspective(45.0, (float)width/(float)height, 1.0, 100.0);

	glViewport( 0, 0, width, height );
}

void my_timer(int v) 
{
	/* calls the display function v times a second */
	glutPostRedisplay();
	glutTimerFunc(1000/v, my_timer, v);
}

int main(int argc, char **argv)
{
  /*set up window for display*/
  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0); 
  glutInitWindowSize(ww, wh);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Transformations Exercise");  

  glewExperimental = GL_TRUE;

	glewInit();
  init();

  glutDisplayFunc(display);
  glutKeyboardFunc(Keyboard);
  glutReshapeFunc(reshape);
  //glutIdleFunc(idle);
  glutSpecialFunc(my_special);
     glutTimerFunc(500, my_timer, 60);

  glutMainLoop();
  return 0;
}