/*
* Assignment 2 (Car) - Ross Anderson
* Assignment 3 (Car with Camera Work)
* SEIS750
* Spring 2012
**/

#include <GL/Angel.h>
#include <math.h>
#pragma comment(lib, "glew32.lib")

//store window width and height
int ww=500, wh=500;

#define M_PI 3.14159265358979323846
typedef enum
{
	CAR,
	STAGE,
	WHEEL,
	HEAD,
	EYE,
	WHEEL_STRIPE,
	WHEEL_CONNECTORS,
	VAO_COUNT
} vertexArrayObjectsEnum;

typedef enum
{
	STATIC_CAMERA,
	VIEWPOINT_CAMERA,
	CHASE_CAMERA,
	NUMBER_OF_CAMERAS
} cameraAnglesEnum;

GLuint vao[VAO_COUNT];
GLuint vbo[VAO_COUNT*2];  // Two vbo's per vao may not always be true, but for now it is

//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;
GLuint vPosition;
GLuint vColor;

/* Not all the t,r,s globals used at this point, but leave them in in case they are
needed for viewing in debug mode */
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
GLdouble headAngle = 0.0f;
GLdouble wheelRollAngle = 0.0f;
GLdouble wheelRollRate = 0.0f;
GLdouble carAngle = 0.0;
GLdouble carAngleRate = 0.0;
GLdouble xPosition = 0.0f;
GLdouble yPosition = 0.0f;
GLdouble zPosition = 0.0f;
GLdouble velocity = 0;

GLdouble zoom = 45;
GLdouble dolly = 50;
cameraAnglesEnum camera = STATIC_CAMERA;
bool staticCameraCenterOfStage = true;

#define CAR_POINT_COUNT 72
vec4 carVerts[CAR_POINT_COUNT];
vec4 carColors[CAR_POINT_COUNT];
#define STAGE_POINT_COUNT 36
vec4 stageVerts[STAGE_POINT_COUNT];
vec4 stageColors[STAGE_POINT_COUNT];
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

#define CAR_WIDTH 2.5
#define CAR_HEIGHT 2.
#define CAR_LENGTH 5
#define WHEEL_RADIUS 1.5
#define WHEEL_THICKNESS 0.5

#define STAGE_WIDTH 25.0
#define STAGE_HEIGHT 1
#define STAGE_DEPTH STAGE_WIDTH
#define HEAD_RADIUS 0.5
#define EYE_RADIUS 0.1

#define WHEEL_X_OFFSET (CAR_WIDTH+1)
#define WHEEL_Y_OFFSET (-CAR_HEIGHT/2)
#define WHEEL_Z_OFFSET (CAR_LENGTH*0.8)

#define DEFAULT_ZOOM 45
#define DEFAULT_DOLLY 50

void generateCar(){
	for(int i=0; i<6; i++){
		carColors[i] = vec4(0.0, 1.0, 1.0, 1.0); //back
	}
	carVerts[0] = vec4(CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[1] = vec4(CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[2] = vec4(-CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[3] = vec4(-CAR_WIDTH, CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[4] = vec4(-CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);
	carVerts[5] = vec4(CAR_WIDTH, -CAR_HEIGHT, CAR_LENGTH, 1.0);


	for(int i=6; i<12; i++){
		carColors[i] = vec4(0.0, 1.0, 0.0, 1.0); //front
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
		carColors[i] = vec4(1.0, 1.0, 1.0, 1.0);
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

	wheelVerts[0] = vec4(0.0, 0.0, WHEEL_THICKNESS, 1.0);
	wheelVerts[0+WHEEL_POINT_COUNT] = vec4(0.0, 0.0, -WHEEL_THICKNESS, 1.0);
	int connectorCount = 0;
	for ( i = 1; i < WHEEL_POINT_COUNT; i++)
	{
		float Angle = (i-1) * (2.0*M_PI/180);
		float X = cos(Angle)*WHEEL_RADIUS;
		float Y = sin(Angle)*WHEEL_RADIUS;
		wheelVerts[i] = vec4(X, Y, 0.5, 1.0);
		wheelVerts[i+WHEEL_POINT_COUNT] = vec4(X, Y, -WHEEL_THICKNESS, 1.0);
		Angle = i * (2.0*M_PI/180);
		float XNext = cos(Angle) * WHEEL_RADIUS;
		float YNext = sin(Angle) * WHEEL_RADIUS;
		wheelConVerts[connectorCount++] = vec4(X, Y, -WHEEL_THICKNESS, 1.0);
		wheelConVerts[connectorCount++] = vec4(XNext, YNext, -WHEEL_THICKNESS, 1.0);
		wheelConVerts[connectorCount++] = vec4(XNext, YNext, WHEEL_THICKNESS, 1.0);
		wheelConVerts[connectorCount++] = vec4(XNext, YNext, WHEEL_THICKNESS, 1.0);
		wheelConVerts[connectorCount++] = vec4(X, Y, WHEEL_THICKNESS, 1.0);
		wheelConVerts[connectorCount++] = vec4(X, Y, -WHEEL_THICKNESS, 1.0);
	}

	for (int i = 0; i < WHEEL_CONNECTOR_POINT_COUNT; i++)
	{
		wheelConColors[i] = vec4(0.05, 0.05, 0.05, 1.0);
	}


	wheelStripeVerts[0] = vec4(-WHEEL_RADIUS*.75, -WHEEL_RADIUS*.75, WHEEL_THICKNESS+0.001, 1.0);
	wheelStripeVerts[1] = vec4(WHEEL_RADIUS*.75, -WHEEL_RADIUS*.75, WHEEL_THICKNESS+0.001, 1.0);
	wheelStripeVerts[2] = vec4(0.0, WHEEL_RADIUS*.75, WHEEL_THICKNESS+0.001, 1.0);
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

/* Since the wheel is kind of complex, use this helper function to draw it 
after the draw position has been set up. */
void drawWheel(void)
{
	glBindVertexArray( vao[WHEEL] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, WHEEL_POINT_COUNT );    // draw the outside wheel 
	glDrawArrays( GL_TRIANGLE_FAN, WHEEL_POINT_COUNT, WHEEL_POINT_COUNT*2 );    // draw the inside wheel 
	glBindVertexArray( vao[WHEEL_STRIPE] );
	glDrawArrays( GL_TRIANGLES, 0, WHEEL_STRIPE_POINT_COUNT );    // draw wheel stripe 
	glBindVertexArray( vao[WHEEL_CONNECTORS] );
	glDrawArrays( GL_TRIANGLES, 0, WHEEL_CONNECTOR_POINT_COUNT );    // draw the connections between the two discs 
}

void display(void)
{
	/*clear all pixels*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// we'll explain this later, but it's setting our default modelview matrix
	if (camera == STATIC_CAMERA)
	{
		if (staticCameraCenterOfStage == true)
		{
			mv = LookAt(vec4(0, CAR_HEIGHT+(WHEEL_RADIUS), dolly, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));
		}
		else
		{
			mv = LookAt(vec4(0, CAR_HEIGHT+(WHEEL_RADIUS), dolly, 1.0), vec4(xPosition, yPosition, zPosition, 1.0), vec4(0, 1, 0, 0.0));
		}
	}
	else if (camera == VIEWPOINT_CAMERA)	
	{
		float Angle = (M_PI*(carAngle+headAngle)/180);
		float X = sin(Angle)*40;
		float Z = cos(Angle)*40;
		mv = LookAt(vec4(xPosition, yPosition+STAGE_HEIGHT+(CAR_HEIGHT*2)+WHEEL_RADIUS+(HEAD_RADIUS*2), zPosition, 1.0), 
			vec4(X+xPosition, 1, Z+zPosition, 1.0), vec4(0, 1, 0, 0.0));
	}
	else if (camera == CHASE_CAMERA)
	{
		mv = LookAt(vec4(50, 50, 50, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));
	}

	mv = mv*Translate(tx, ty, tz);
	mv = mv*RotateX(rx);
	mv = mv*RotateY(ry);
	mv = mv*RotateZ(rz);
	mv = mv*Scale(sx, sy, sz);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	// and we also need to send our projection matrix, which again is more appropriately
	// a uniform instead of an attribute since it's the same for every vertex
	p = Perspective(zoom, (float)ww/(float)wh, 1.0, 100.0);

	if (camera == VIEWPOINT_CAMERA)
	{
		p = Perspective(zoom, (float)ww/(float)wh, (HEAD_RADIUS*2+(EYE_RADIUS*2)), 100.0);
		//p = p*Translate(xPosition+10, yPosition+10, zPosition+10);
		//p = p*RotateY(9);
		//p = p*Translate(-50, -50, -50);
		//p = p*Translate(0.0, CAR_HEIGHT+(WHEEL_RADIUS), 0.0);
		//p = p*Translate(xPosition, yPosition, zPosition);
		//p = p*RotateY(carAngle);
		//p = p*Translate(0.0, CAR_HEIGHT+(HEAD_RADIUS*2), 0.0);
		//p = p*RotateY(headAngle);
	}

	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	glBindVertexArray( vao[STAGE] );
	glDrawArrays( GL_TRIANGLES, 0, STAGE_POINT_COUNT );    // draw the stage

	mv = mv*Translate(0.0, CAR_HEIGHT+(WHEEL_RADIUS), 0.0);
	mv = mv*Translate(xPosition, yPosition, zPosition);
	mv = mv*RotateY(carAngle);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	
	glBindVertexArray( vao[CAR] );
	glDrawArrays( GL_TRIANGLES, 0, CAR_POINT_COUNT );    // draw the car 

	mat4 original = mv;

	mv = mv*Translate(0.0, CAR_HEIGHT+(HEAD_RADIUS*2), 0.0);
	mv = mv*RotateY(headAngle);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[HEAD] );
	glDrawArrays( GL_LINE_LOOP, 0, HEAD_POINT_COUNT );    // draw the head 
	mat4 headOriginal = mv;
	mv = mv*Translate(.4*HEAD_RADIUS, 0.0, HEAD_RADIUS);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[EYE] );
	glDrawArrays( GL_LINE_LOOP, 0, EYE_POINT_COUNT );    // draw one eye 
	mv = headOriginal;
	mv = mv*Translate(-.4*HEAD_RADIUS, 0.0, HEAD_RADIUS);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[EYE] );
	glDrawArrays( GL_LINE_LOOP, 0, EYE_POINT_COUNT );    // draw the other eye 

	mv = original;

	mv = mv*Translate(WHEEL_X_OFFSET, WHEEL_Y_OFFSET, -WHEEL_Z_OFFSET);
	mv = mv*RotateY(90-steering);
	mv = mv*RotateZ(wheelRollAngle);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	drawWheel();

	mv = original;
	mv = mv*Translate(-WHEEL_X_OFFSET, WHEEL_Y_OFFSET, -WHEEL_Z_OFFSET);
	mv = mv*RotateY(-90-steering);
	mv = mv*RotateZ(-wheelRollAngle);
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
	mv = mv*RotateZ(-wheelRollAngle);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	drawWheel();

	glutSwapBuffers();
}

void stopCar(void)
{
	carAngleRate = 0;
	wheelRollRate = 0;
	velocity = 0;
}
/* Function call for game reset */
void my_special(int key, int x, int y) 
{

#define MAX_VELOCITY 5
#define MAX_STEERING 35

	if (key == GLUT_KEY_UP)
	{
		if (velocity > -MAX_VELOCITY)
		{
			velocity -= 1.0;
		}
	}

	if (key == GLUT_KEY_DOWN)
	{
		if (velocity < MAX_VELOCITY)
		{
			velocity+= 1.0;
		}
	}

	if (key == GLUT_KEY_RIGHT)
	{
		if (steering < MAX_STEERING)
		{
			steering += 1.0;
		}
	}

	if (key == GLUT_KEY_LEFT)
	{
		if (steering > -MAX_STEERING)
		{
			steering -= 1.0;
		}
	}

	wheelRollRate = velocity*2;		// Roll it twice as fast as its not very obvious
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
	/*exit when the escape key is pressed*/
	if (key == 27)
		exit(0);

	if (key == 'z')
	{
		headAngle -= 2.0;
	}
	if (key == 'x')
	{
		headAngle += 2.0;
	}
	if (key == ' ')
	{
		stopCar();
	}

	if (camera == STATIC_CAMERA)
		{
		if (key == 'a')
		{
			zoom -= 1.0;
		}
		if (key == 's')
		{
			zoom += 1.0;
		}
		if (key == 'w')
		{
			dolly +=1.0;
		}
		if (key == 'q')
		{
			dolly -= 1.0;
		}
		if (key == 'f')
		{
			staticCameraCenterOfStage = !staticCameraCenterOfStage;
		}
	}

	if (key == 'r')
	{
		zoom = DEFAULT_ZOOM;
		dolly = DEFAULT_DOLLY;
	}

	if (key == 'c')
	{
		camera = (cameraAnglesEnum) ((int) camera + 1);

		if (camera >= NUMBER_OF_CAMERAS)
		{
			camera = STATIC_CAMERA;
		}
	}



#ifdef DEBUG		// Used for creating objects to see all sides
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
#endif

	glutPostRedisplay();
}

void init() 
{
	int vboIndex = 0;	// Used to auto-increment the vbo index
	/*select clearing (background) color*/
	glClearColor(0.0, 0.0, 0.0, 0.0);

	//populate our arrays
	generateCar();
	generateStage();
	generateWheel();
	generateHead();
	generateEye();

	// Load shaders and use the resulting shader program
	GLuint program = InitShader( "vshader-transform.glsl", "fshader-transform.glsl" );
	glUseProgram( program );

	// Create all vertex array object
	glGenVertexArrays( VAO_COUNT, &vao[0] );
	glGenBuffers(VAO_COUNT*2, &vbo[0]);

	// Create and initialize any buffer objects
	glBindVertexArray( vao[CAR] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carVerts), carVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carColors), carColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray( vao[STAGE] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(stageVerts), stageVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(stageColors), stageColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray( vao[WHEEL] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelVerts), wheelVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelColors), wheelColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray( vao[HEAD] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(headVerts), headVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(headColors), headColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray( vao[EYE] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(eyeVerts), eyeVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(eyeColors), eyeColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray( vao[WHEEL_STRIPE] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelStripeVerts), wheelStripeVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelStripeColors), wheelStripeColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray( vao[WHEEL_CONNECTORS] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelConVerts), wheelConVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
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
	p = Perspective(zoom, (float)ww/(float)wh, 1.0, 100.0);

	glViewport( 0, 0, width, height );
}

void my_timer(int v) 
{	
	/* Avoid checking for 0.0, since we rely on integers for now just make sure we
	are no where near -1 or 1 to determine if we are in motion. */
	if (velocity > 0.5 || velocity < -0.5)
	{
		wheelRollAngle += wheelRollRate;
		xPosition += sin((M_PI*carAngle)/180) * velocity*0.1;	// 0.1 is used to throttle the speed back.
		zPosition += cos((M_PI*carAngle)/180) * velocity*0.1;

		float temp = steering;
		if (velocity < 0)
		{
			temp *= -1;
		}
		carAngle += temp * 0.03;	// Throttle back the angle as well.
	}

	/* Checks to see if we are off the stage.  
	Stop the car and try to move back on stage if off the edge */
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
	glutSpecialFunc(my_special);
	glutTimerFunc(500, my_timer, 60);

	glutMainLoop();
	return 0;
}