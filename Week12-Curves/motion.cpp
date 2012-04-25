/*
 *Skeleton program
 *Renders a ball that moves along a path using linear interpolation, CR-Splines, or Bezier curves
 *Note that this is highly unoptomized code and should be for educational purposes only
 *SEIS750
 *Spring 2012
 **/

#include <GL/Angel.h>
#include <math.h>

#pragma comment(lib, "glew32.lib")

//store window width and height
int ww=1000, wh=1000;
#define NUM_POINTS 24 //how many control points should there be? (for Bezier curves, try to make this a multiple of 3)
#define M_PI 3.1415926535898
//stores x,y coordinates for 3 triangle verticies
vec2 verts[NUM_POINTS]; //store x and y location for each control point
int p0 = 0; //Control point index 0 is the starting P0
float t = 0; //t is the free parameter
float tstep = 0.01; //delta t

GLuint vao[1];
GLuint vbo[1];

//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view, cmv;
GLuint projection, cp;
GLuint vPosition, vColor;
GLuint shader_t;
GLuint controlPoints;
GLuint control, CR, lerp, Bezier, bspline, active;

int max(int a, int b){
	return a>b ? a : b;
}

int min(int a, int b){
	return a<b ? a : b;
}

void display(void)
{
  /*clear all pixels*/
  glClear(GL_COLOR_BUFFER_BIT);
  
  /*draw Control Points*/
  glUseProgram(control);
  //Note that in this case, we are keeping separate local handles for separate shader programs
  //cp and cmv store handles for this shader program, projection and model_view are for the other shader program
  glUniformMatrix4fv(cp, 1, GL_TRUE, p);
  glUniformMatrix4fv(cmv, 1, GL_TRUE, mat4(1.0));
  glPointSize(10);
  glVertexAttrib4fv(vColor, vec4(1, 0, 0, 1)); //any leading control points are red
  glDrawArrays(GL_POINTS, 0, max(0,(p0+4)-NUM_POINTS));
  glVertexAttrib4fv(vColor, vec4(0, 1, 0, 1)); //non-control points
  glDrawArrays(GL_POINTS, max(0,(p0+4)-NUM_POINTS), p0 - max(0,(p0+4)-NUM_POINTS));
  glVertexAttrib4fv(vColor, vec4(1, 0, 0, 1)); //control points (possibly to end of array)
  glDrawArrays(GL_POINTS, p0, min(NUM_POINTS-p0, 4));
  glVertexAttrib4fv(vColor, vec4(0, 1, 0, 1)); //any remaining non-control points
  glDrawArrays(GL_POINTS, (p0+4)%NUM_POINTS, max(0, NUM_POINTS - p0 - 4));
  
  //The following block of code does a really poor job of drawing the Bezier guide lines
  //barely useful
 /* glVertexAttrib4fv(vColor, vec4(0,0,0,1));
  for(int i = 0; i < NUM_POINTS-4; i+=3){
	  glDrawArrays(GL_LINES, i, 4);
  }*/
	   
  //Draw the ball
  glUseProgram(active);
  glUniformMatrix4fv(projection, 1, GL_TRUE, p);
  glUniformMatrix4fv(model_view, 1, GL_TRUE, mat4(1.0));
  glPointSize(20);
  //now it's time for our little ball that's moving around according to our active curve program
  glBegin(GL_POINTS);
	glVertex2f(0,0); //position is ignored anyway, we just need a vertex to get sent down the pipeline to
					//trigger a run of the vertex shader program
  glEnd();


  glutSwapBuffers();
}

void setupShader(GLuint prog){
	glUseProgram( prog );
	
	//grab pointers for our modelview and perspecive uniform matrices
	model_view = glGetUniformLocation(prog, "model_view");
	projection = glGetUniformLocation(prog, "projection");
	
	//and our moving dot shader needs these uniforms as well
	shader_t = glGetUniformLocation(prog, "t");
	controlPoints = glGetUniformLocation(prog, "controlPoints");

}

void Keyboard(unsigned char key, int x, int y) {
	/*exit when the escape key is pressed*/
	if (key == 27)
		exit(0);

	if (key == '1'){
		active = lerp;
		glutSetWindowTitle("Linear Interpolation");
	}
	if (key == '2'){
		active = CR;
		glutSetWindowTitle("CR-Spline");
	}if (key == '3'){
		active = Bezier;
		glutSetWindowTitle("Bezier Curves");
	}if (key == '4'){
		active = bspline;
		glutSetWindowTitle("Cubic B-Spline");
	}
	
	//if all shader programs have the exact same attribute and uniform list, we will usually
	//get away without needing to fetch the uniform and attribute locations when we switch programs,
	//but let's do it anyway just to be safe
	setupShader(active);
	glutPostRedisplay();

}



//Animate
void timer(int fps){
	t+=tstep; //Increase to next t value
	if(t >= 1){//Move to next set of control points
		t-=1;
		if(active == Bezier){//bezier moves 4 at a time
			p0 = (p0+3)%NUM_POINTS;
		}else{
			p0= (p0+1)%NUM_POINTS;//advance to next set of control points
		}
		vec2 points[4] = {verts[p0], verts[(p0+1)%NUM_POINTS], verts[(p0+2)%NUM_POINTS], verts[(p0+3)%NUM_POINTS]};
		glUniform2fv(controlPoints, 4, *points); 
		printf("p0: %d\n", p0);
	}
	glUniform1f(shader_t, t);
	//figure out new position of ball
	
	glutPostRedisplay();

	//recursive call
	glutTimerFunc(1000/fps, timer, fps);
}




void init() {
  /*select clearing (background) color*/
  glClearColor(1.0, 1.0, 1.0, 1.0); //white background
  glEnable(GL_POINT_SMOOTH); //so we get circles instead of squares


  //initialize control point positions to two concentric circles
  for(int i=0; i<NUM_POINTS; i++){
	  verts[i] = vec2(cos(2*M_PI*(float)i/NUM_POINTS)*(250+100*(i%2))+500, sin(2*M_PI*(float)i/NUM_POINTS)*(250+100*(i%2))+500);
  }
  


    // Load shaders and use the resulting shader program
    control = InitShader( "vshader-control.glsl", "fshader-universal.glsl" );
	lerp = InitShader( "vshader-lerp.glsl", "fshader-universal.glsl" );
	CR = InitShader( "vshader-CR.glsl", "fshader-universal.glsl" );
	Bezier = InitShader( "vshader-Bezier.glsl", "fshader-universal.glsl" );
	bspline = InitShader( "vshader-b-spline.glsl", "fshader-universal.glsl" );
    active = lerp;
	setupShader(active);
	// Create a vertex array object
    glGenVertexArrays( 1, &vao[0] );

    // Create and initialize any buffer objects
	glBindVertexArray( vao[0] );
	glGenBuffers( 1, &vbo[0] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(control, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);

	
	
	//grab pointers for our modelview and perspecive uniform matrices for the control point shader
	cmv = glGetUniformLocation(control, "model_view");
	cp = glGetUniformLocation(control, "projection");
	
	//and our control point shader attributes
	vPosition = glGetAttribLocation(control, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
	vColor = glGetAttribLocation(control, "vColor");
	

}

void reshape(int width, int height){
	 p = Ortho2D(0, width, 0, height);

	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glViewport( 0, 0, width, height );
}


int main(int argc, char **argv)
{
  /*set up window for display*/
  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0); 
  glutInitWindowSize(ww, wh);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutCreateWindow("Week 12 Exercise - Linear Interpolation");  
  
  glewExperimental = GL_TRUE;

	glewInit();
  init();

  glutDisplayFunc(display);
  glutKeyboardFunc(Keyboard);
  glutTimerFunc(100, timer, 60);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;
}