/*
* Assignment 2 (Car) - Ross Anderson
* SEIS750
* Spring 2012
**/

#include <GL/Angel.h>
#include <math.h>
#pragma comment(lib, "glew32.lib")

//store window width and height
int ww=500, wh=500;

#define M_PI 3.14159265358979323846

GLuint vao[10];
GLuint vbo[20];

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

GLdouble steering = 0.0f;
GLdouble headAngle = 180.0f;
GLdouble wheelRollAngle = 0.0f;
GLdouble wheelRollRate = 0.0f;
GLdouble carAngle = 0.0;
GLdouble carAngleRate = 0.0;
GLdouble xPosition = 0.0f;
GLdouble yPosition = 0.0f;
GLdouble zPosition = 0.0f;
GLdouble xRate = 0.0f;
GLdouble yRate = 0.0f;
GLdouble zRate = 0.0f;
int velocity = 0;

#define CAR_POINT_COUNT 72
vec4 carVerts[CAR_POINT_COUNT];
vec4 carColors[CAR_POINT_COUNT];
vec4 stageVerts[36];
vec4 stageColors[36];
#define WHEEL_POINT_COUNT 362
vec4 wheelVerts[WHEEL_POINT_COUNT*2];
vec4 wheelColors[WHEEL_POINT_COUNT*2];
#define WHEEL_STRIPE_POINT_COUNT 3
vec4 wheelStripeVerts[WHEEL_STRIPE_POINT_COUNT];
vec4 wheelStripeColors[WHEEL_STRIPE_POINT_COUNT];
#define WHEEL_CONNECTOR_POINT_COUNT 360*6
vec4 wheelConVerts[WHEEL_CONNECTOR_POINT_COUNT];
vec4 wheelConColors[WHEEL_CONNECTOR_POINT_COUNT];
#define HEAD_POINT_COUNT 342
vec4 headVerts[HEAD_POINT_COUNT];
vec4 headColors[HEAD_POINT_COUNT];
#define EYE_POINT_COUNT 342
vec4 eyeVerts[EYE_POINT_COUNT];
vec4 eyeColors[EYE_POINT_COUNT];

#define CAR_WIDTH 5
#define CAR_HEIGHT 5
#define CAR_LENGTH 10

#define STAGE_WIDTH 50.0
#define STAGE_HEIGHT 1
#define STAGE_DEPTH STAGE_WIDTH
#define HEAD_RADIUS 1
#define EYE_RADIUS 0.2

void generatecar(){
	for(int i=0; i<6; i++){
		carColors[i] = vec4(0.0, 1.0, 1.0, 1.0); //front
	}
	carVerts[0] = vec4(CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[1] = vec4(CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[2] = vec4(-CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[3] = vec4(-CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[4] = vec4(-CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[5] = vec4(CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);


	for(int i=6; i<12; i++){
		carColors[i] = vec4(1.0, 0.0, 1.0, 1.0); //back
	}
	carVerts[6] = vec4(-CAR_WIDTH, -CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[7] = vec4(-CAR_WIDTH, CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[8] = vec4(CAR_WIDTH, CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[9] = vec4(CAR_WIDTH, CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[10] = vec4(CAR_WIDTH, -CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[11] = vec4(-CAR_WIDTH, -CAR_HEIGHT, -CAR_LENGTH, 1.0);

	for(int i=12; i<18; i++){
		carColors[i] = vec4(1.0, 1.0, 0.0, 1.0); //left
	}
	carVerts[12] = vec4(CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[13] = vec4(CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[14] = vec4(CAR_WIDTH, -CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[15] = vec4(CAR_WIDTH, -CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[16] = vec4(CAR_WIDTH, CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[17] = vec4(CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);

	for(int i=18; i<24; i++){
		carColors[i] = vec4(1.0, 0.0, 0.0, 1.0); //right
	}
	carVerts[18] = vec4(-CAR_WIDTH, CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[19] = vec4(-CAR_WIDTH, -CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[20] = vec4(-CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[21] = vec4(-CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[22] = vec4(-CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[23] = vec4(-CAR_WIDTH, CAR_HEIGHT, -CAR_LENGTH, 1.0);

	for(int i=24; i<30; i++){
		carColors[i] = vec4(0.0, 0.0, 1.0, 1.0); //top
	}
	carVerts[24] = vec4(CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[25] = vec4(CAR_WIDTH, CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[26] = vec4(-CAR_WIDTH, CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[27] = vec4(-CAR_WIDTH, CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[28] = vec4(-CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[29] = vec4(CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);

	for(int i=30; i<36; i++){
		carColors[i] = vec4(0.0, 1.0, 0.0, 1.0); //bottom
	}
	carVerts[30] = vec4(CAR_WIDTH, -CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[31] = vec4(CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[32] = vec4(-CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[33] = vec4(-CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[34] = vec4(-CAR_WIDTH, -CAR_HEIGHT, -CAR_LENGTH, 1.0);
	carVerts[35] = vec4(CAR_WIDTH, -CAR_HEIGHT, -CAR_LENGTH, 1.0);

	for(int i=36; i<72; i++){
		carColors[i] = vec4(1.0, 1.0, 1.0, 1.0); //front
	}

	int i = 36;
	// Front Racing Stipes
	carVerts[i++] = vec4(CAR_WIDTH*.3, -CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.3, CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.6, CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.6, CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.3, -CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.6, -CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.3, -CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.3, CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.6, CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.6, CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.3, -CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.6, -CAR_HEIGHT, CAR_LENGTH+.001, 1.0);
	
	// Middle Racing Stripes
	carVerts[i++] = vec4(CAR_WIDTH*.3, CAR_HEIGHT+.001, -CAR_LENGTH, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.3, CAR_HEIGHT+.001, CAR_LENGTH, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.6, CAR_HEIGHT+.001, CAR_LENGTH, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.6, CAR_HEIGHT+.001, CAR_LENGTH, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.3, CAR_HEIGHT+.001, -CAR_LENGTH, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.6, CAR_HEIGHT+.001, -CAR_LENGTH, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.3, CAR_HEIGHT+.001, -CAR_LENGTH, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.3, CAR_HEIGHT+.001, CAR_LENGTH, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.6, CAR_HEIGHT+.001, CAR_LENGTH, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.6, CAR_HEIGHT+.001, CAR_LENGTH, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.3, CAR_HEIGHT+.001, -CAR_LENGTH, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.6, CAR_HEIGHT+.001, -CAR_LENGTH, 1.0);

	// Back Racing Stripes
	carVerts[i++] = vec4(CAR_WIDTH*.3, -CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.3, CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.6, CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.6, CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.3, -CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
	carVerts[i++] = vec4(CAR_WIDTH*.6, -CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.3, -CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.3, CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.6, CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.6, CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.3, -CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
	carVerts[i++] = vec4(-CAR_WIDTH*.6, -CAR_HEIGHT, -CAR_LENGTH-.001, 1.0);
}
void generateStage(){
	for(int i=0; i<36; i++){
		stageColors[i] = vec4(0.25, 0.25, 0.25, 1.0); //front
	}
	stageVerts[0] = vec4(STAGE_WIDTH, -STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[1] = vec4(STAGE_WIDTH, STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[2] = vec4(-STAGE_WIDTH, STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[3] = vec4(-STAGE_WIDTH, STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[4] = vec4(-STAGE_WIDTH, -STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[5] = vec4(STAGE_WIDTH, -STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[6] = vec4(-STAGE_WIDTH, -STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[7] = vec4(-STAGE_WIDTH, STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[8] = vec4(STAGE_WIDTH, STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[9] = vec4(STAGE_WIDTH, STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[10] = vec4(STAGE_WIDTH, -STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[11] = vec4(-STAGE_WIDTH, -STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[12] = vec4(STAGE_WIDTH, STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[13] = vec4(STAGE_WIDTH, -STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[14] = vec4(STAGE_WIDTH, -STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[15] = vec4(STAGE_WIDTH, -STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[16] = vec4(STAGE_WIDTH, STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[17] = vec4(STAGE_WIDTH, STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[18] = vec4(-STAGE_WIDTH, STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[19] = vec4(-STAGE_WIDTH, -STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[20] = vec4(-STAGE_WIDTH, -STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[21] = vec4(-STAGE_WIDTH, -STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[22] = vec4(-STAGE_WIDTH, STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[23] = vec4(-STAGE_WIDTH, STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[24] = vec4(STAGE_WIDTH, STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[25] = vec4(STAGE_WIDTH, STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[26] = vec4(-STAGE_WIDTH, STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[27] = vec4(-STAGE_WIDTH, STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[28] = vec4(-STAGE_WIDTH, STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[29] = vec4(STAGE_WIDTH, STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[30] = vec4(STAGE_WIDTH, -STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[31] = vec4(STAGE_WIDTH, -STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[32] = vec4(-STAGE_WIDTH, -STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[33] = vec4(-STAGE_WIDTH, -STAGE_HEIGHT, STAGE_DEPTH, 1.0);
	stageVerts[34] = vec4(-STAGE_WIDTH, -STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
	stageVerts[35] = vec4(STAGE_WIDTH, -STAGE_HEIGHT, -STAGE_DEPTH, 1.0);
}
void generateWheel()
{
	int i;
	for (i = 0; i < WHEEL_POINT_COUNT; i++)
	{
		wheelColors[i] = vec4(0.75, 0.75, 0.75, 1.0);
	}
	for ( ; i < WHEEL_POINT_COUNT*2; i++)
	{
		wheelColors[i] = vec4(0.75, 0.75, 0.75, 1.0);
	}

	wheelVerts[0] = vec4(0.0, 0.0, 0.5, 1.0);
	wheelVerts[0+WHEEL_POINT_COUNT] = vec4(0.0, 0.0, -0.5, 1.0);
	int connectorCount = 0;
	for ( i = 1; i < WHEEL_POINT_COUNT; i++)
	{
#define RADIUS 3
		float Angle = (i-1) * (2.0*M_PI/180);
		float X = cos( Angle )*RADIUS;
		float Y = sin( Angle )*RADIUS;
		wheelVerts[i] = vec4(X, Y, 0.5, 1.0);
		wheelVerts[i+WHEEL_POINT_COUNT] = vec4(X, Y, -0.5, 1.0);
		Angle = (i) * (2.0*M_PI/(WHEEL_POINT_COUNT-1));
		float XNext = cos( Angle ) * RADIUS;
		float YNext = sin( Angle ) * RADIUS;
		wheelConVerts[connectorCount++] = vec4(X, Y, -0.5, 1.0);
		wheelConVerts[connectorCount++] = vec4(XNext, YNext, -0.5, 1.0);
		wheelConVerts[connectorCount++] = vec4(XNext, YNext, 0.5, 1.0);
		wheelConVerts[connectorCount++] = vec4(XNext, YNext, 0.5, 1.0);
		wheelConVerts[connectorCount++] = vec4(X, Y, 0.5, 1.0);
		wheelConVerts[connectorCount++] = vec4(X, Y, -0.5, 1.0);
	}

	for (int i = 0; i < WHEEL_CONNECTOR_POINT_COUNT; i++)
	{
		wheelConColors[i] = vec4(0.05, 0.05, 0.05, 1.0);
	}


	wheelStripeVerts[0] = vec4(-1.5, -1.5, 0.51, 1.0);
	wheelStripeVerts[1] = vec4(1.5, -1.5, 0.51, 1.0);
	wheelStripeVerts[2] = vec4(0.0, 1.5, 0.51, 1.0);
	wheelStripeColors[0] = vec4(0.0, 0.0, 0.0, 1.0);
	wheelStripeColors[1] = vec4(0.0, 0.0, 0.0, 1.0);
	wheelStripeColors[2] = vec4(0.0, 0.0, 0.0, 1.0);

}
void generateHead()
{
	int k = 0;
	for (float phi = -80.0; phi <= 80.0; phi += 20.0)
	{
		float phir = phi*(M_PI/180);
		float phir20 = (phi + 20)*(M_PI/180);

		for (float theta = -180.0; theta <= 180.0; theta += 20.0)
		{
			float thetar = theta*(M_PI/180);
			headVerts[k] = vec4(sin(thetar)*cos(phir)*HEAD_RADIUS, cos(thetar)*cos(phir)*HEAD_RADIUS, sin(phir)*HEAD_RADIUS, 1.0);
			headColors[k++] = vec4(1.0, 1.0, 1.0, 1.0);
			headVerts[k] = vec4(sin(thetar)*cos(phir20)*HEAD_RADIUS, cos(thetar)*cos(phir20)*HEAD_RADIUS, sin(phir20)*HEAD_RADIUS, 1.0);
			headColors[k++] = vec4(1.0, 1.0, 1.0, 1.0);
		}
	}
}
void generateEye()
{

	int k = 0;
	for (float phi = -80.0; phi <= 80.0; phi += 20.0)
	{
		float phir = phi*(M_PI/180);
		float phir20 = (phi + 20)*(M_PI/180);

		for (float theta = -180.0; theta <= 180.0; theta += 20.0)
		{
			float thetar = theta*(M_PI/180);
			eyeVerts[k] = vec4(sin(thetar)*cos(phir)*EYE_RADIUS, cos(thetar)*cos(phir)*EYE_RADIUS, sin(phir*EYE_RADIUS), 1.0);
			eyeColors[k++] = vec4(0.0, 0.0, 0.0, 0.0);
			eyeVerts[k] = vec4(sin(thetar)*cos(phir20)*EYE_RADIUS, cos(thetar)*cos(phir20)*EYE_RADIUS, sin(phir20*EYE_RADIUS), 1.0);
			eyeColors[k++] = vec4(0.0, 0.0, 0.0, 0.0);
		}
	}
}

void drawWheel(void)
{
	glBindVertexArray( vao[2] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, WHEEL_POINT_COUNT );    // draw the wheel 
	glDrawArrays( GL_TRIANGLE_FAN, WHEEL_POINT_COUNT, WHEEL_POINT_COUNT*2 );    // draw the wheel 
	glBindVertexArray( vao[5] );
	glDrawArrays( GL_TRIANGLES, 0, WHEEL_STRIPE_POINT_COUNT );    // draw the wheel 
	glBindVertexArray( vao[6] );
	glDrawArrays( GL_TRIANGLES, 0, WHEEL_CONNECTOR_POINT_COUNT );    // draw the wheel 
}

void display(void)
{
	/*clear all pixels*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// we'll explain this later, but it's setting our default modelview matrix
	mv = LookAt(vec4(0, 100, 100, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));

	mv = mv*Translate(tx, ty, tz);

	mv = mv*RotateX(rx);
	mv = mv*RotateY(ry);
	mv = mv*RotateZ(rz);

	mv = mv*Scale(sx, sy, sz);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	// and we also need to send our projection matrix, which again is more appropriately
	// a uniform instead of an attribute since it's the same for every vertex
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	glBindVertexArray( vao[1] );
	glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the stage 

	mv = mv*Translate(0.0, 10, 0.0);
	mv = mv*Translate(xPosition, yPosition, zPosition);
	mv = mv*RotateY(carAngle);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	
	glBindVertexArray( vao[0] );
	glDrawArrays( GL_TRIANGLES, 0, CAR_POINT_COUNT );    // draw the car 

	mat4 original = mv;

	mv = mv*Translate(0.0, CAR_HEIGHT+(HEAD_RADIUS*2), 0.0);
	mv = mv*RotateY(headAngle);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[3] );
	glDrawArrays( GL_LINE_LOOP, 0, HEAD_POINT_COUNT );    // draw the head 
	mat4 headOriginal = mv;
	mv = mv*Translate(.4*HEAD_RADIUS, 0.0, HEAD_RADIUS);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[4] );
	glDrawArrays( GL_LINE_LOOP, 0, EYE_POINT_COUNT );    // draw the eye 
	mv = headOriginal;
	mv = mv*Translate(-.4*HEAD_RADIUS, 0.0, HEAD_RADIUS);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[4] );
	glDrawArrays( GL_LINE_LOOP, 0, EYE_POINT_COUNT );    // draw the eye 

	mv = original;
#define WHEEL_X_OFFSET (CAR_WIDTH+1)
#define WHEEL_Y_OFFSET -5.0
#define WHEEL_Z_OFFSET (CAR_LENGTH*0.8)

	mv = mv*Translate(WHEEL_X_OFFSET, WHEEL_Y_OFFSET, -WHEEL_Z_OFFSET);
	mv = mv*RotateY(90-steering);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);



	drawWheel();

	mv = original;
	mv = mv*Translate(-WHEEL_X_OFFSET, WHEEL_Y_OFFSET, -WHEEL_Z_OFFSET);
	mv = mv*RotateY(-90-steering);
	mv = mv*RotateZ(wheelRollAngle);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	drawWheel();

	mv = original;
	mv = mv*Translate(WHEEL_X_OFFSET, WHEEL_Y_OFFSET, WHEEL_Z_OFFSET);
	mv = mv*RotateY(90);
	mv = mv*RotateZ(wheelRollAngle);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	drawWheel();

	mv = original;
	mv = mv*Translate(-WHEEL_X_OFFSET, WHEEL_Y_OFFSET, WHEEL_Z_OFFSET);
	mv = mv*RotateY(-90);
	mv = mv*RotateZ(wheelRollAngle);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	drawWheel();

	glutSwapBuffers();
}

void stopCar(void)
{
	xRate = 0;
	yRate = 0;
	zRate = 0;
	carAngleRate = 0;
	wheelRollRate = 0;
	velocity = 0;
}
/* Function call for game reset */
void my_special(int key, int x, int y) 
{

#define MAX_VELOCITY 5

	if (key == GLUT_KEY_UP)
	{
		if (velocity > -MAX_VELOCITY)
		{
			velocity--;
		}
	}

	if (key == GLUT_KEY_DOWN)
	{
		if (velocity < MAX_VELOCITY)
		{
			velocity++;
		}
	}

	if (key == GLUT_KEY_RIGHT)
	{
		if (steering < 35)
		{
			steering += 1.0;
		}
	}

	if (key == GLUT_KEY_LEFT)
	{
		if (steering > -35)
		{
			steering -= 1.0;
		}
	}
#define THROTTLE 0.05
	zRate = velocity- (velocity * (steering/45));
	xRate = velocity-zRate;
	xRate *= THROTTLE;
	zRate *= THROTTLE;
	carAngleRate = steering * THROTTLE;
	wheelRollRate = -1*velocity;
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
	/*exit when the escape key is pressed*/
	if (key == 27)
		exit(0);

	if (key == 'z')
	{
		headAngle -= 1.0;
	}
	if (key == 'x')
	{
		headAngle += 1.0;
	}
	if (key == ' ')
	{
		stopCar();
	}

	if (key == 'a')
	{
		tz -= 1.0;
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
	generatecar();
	generateStage();
	generateWheel();
	generateHead();
	generateEye();

	// Load shaders and use the resulting shader program
	GLuint program = InitShader( "vshader-transform.glsl", "fshader-transform.glsl" );
	glUseProgram( program );

	// Create a vertex array object
	glGenVertexArrays( 1, &vao[0] );

	// Create and initialize any buffer objects
	glBindVertexArray( vao[0] );
	glGenBuffers( 2, &vbo[0] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carVerts), carVerts, GL_STATIC_DRAW);
	// notice that since position is unique for every vertex, we treat it as an 
	// attribute instead of a uniform
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carColors), carColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Create a vertex array object
	glGenVertexArrays( 1, &vao[1] );

	// Create and initialize any buffer objects
	glBindVertexArray( vao[1] );
	glGenBuffers( 2, &vbo[2] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[2] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(stageVerts), stageVerts, GL_STATIC_DRAW);
	// notice that since position is unique for every vertex, we treat it as an 
	// attribute instead of a uniform
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[3] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(stageColors), stageColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Create a vertex array object
	glGenVertexArrays( 1, &vao[2] );

	// Create and initialize any buffer objects
	glBindVertexArray( vao[2] );
	glGenBuffers( 2, &vbo[4] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[4] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelVerts), wheelVerts, GL_STATIC_DRAW);
	// notice that since position is unique for every vertex, we treat it as an 
	// attribute instead of a uniform
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[5] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelColors), wheelColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Create a vertex array object
	glGenVertexArrays( 1, &vao[3] );

	// Create and initialize any buffer objects
	glBindVertexArray( vao[3] );
	glGenBuffers( 2, &vbo[6] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[6] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(headVerts), headVerts, GL_STATIC_DRAW);
	// notice that since position is unique for every vertex, we treat it as an 
	// attribute instead of a uniform
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[7] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(headColors), headColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);


		// Create a vertex array object
	glGenVertexArrays( 1, &vao[4] );

	// Create and initialize any buffer objects
	glBindVertexArray( vao[4] );
	glGenBuffers( 2, &vbo[8] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[8] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(eyeVerts), eyeVerts, GL_STATIC_DRAW);
	// notice that since position is unique for every vertex, we treat it as an 
	// attribute instead of a uniform
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[9] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(eyeColors), eyeColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Create a vertex array object
	glGenVertexArrays( 1, &vao[5] );

	// Create and initialize any buffer objects
	glBindVertexArray( vao[5] );
	glGenBuffers( 2, &vbo[10] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[10] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelStripeVerts), wheelStripeVerts, GL_STATIC_DRAW);
	// notice that since position is unique for every vertex, we treat it as an 
	// attribute instead of a uniform
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[11] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelStripeColors), wheelStripeColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Create a vertex array object
	glGenVertexArrays( 1, &vao[6] );

	// Create and initialize any buffer objects
	glBindVertexArray( vao[6] );
	glGenBuffers( 2, &vbo[12] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[12] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelConVerts), wheelConVerts, GL_STATIC_DRAW);
	// notice that since position is unique for every vertex, we treat it as an 
	// attribute instead of a uniform
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[13] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelConColors), wheelConColors, GL_STATIC_DRAW );
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
	
	if (velocity != 0)
	{
		wheelRollAngle += wheelRollRate;
		xPosition += xRate;
		yPosition += yRate;
		zPosition += zRate;
		carAngle += carAngleRate;
	}

	if ((xPosition+CAR_WIDTH) > STAGE_WIDTH)
	{
		stopCar();
		xPosition = STAGE_WIDTH - CAR_WIDTH-1;
	}

	if((xPosition-CAR_WIDTH) < -STAGE_WIDTH)
	{
		stopCar();
		xPosition = -STAGE_WIDTH + CAR_WIDTH+1;
	}

	if((zPosition+CAR_LENGTH) > STAGE_DEPTH)
	{
		stopCar();
		zPosition = STAGE_DEPTH - CAR_LENGTH-1;
	}

	if ((zPosition-CAR_LENGTH) < -STAGE_DEPTH)
	{
		stopCar();
		zPosition = -STAGE_DEPTH + CAR_LENGTH+1;
	}





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
	glutCreateWindow("Assignment 2 - Ross Anderson");  

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