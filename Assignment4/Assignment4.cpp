/*
* Assignment 2 (Car) - Ross Anderson
* Assignment 3 (Car with Camera Work)
* Assignment 4 (Car with Camera Work and Lighting)
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
	PYLON,
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
GLuint vbo[VAO_COUNT*6];

//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;
GLuint vPosition;
GLuint vColor;
GLuint vNormal;
GLuint vAmbient;
GLuint vDiffuse;
GLuint vSpecular;

//Ambient Light Pointers
GLuint ambient_light;
GLuint vAmbientDiffuseColor;

GLuint light_position[4];
GLuint light_color[4];
GLuint light_direction[4];
GLuint light_cuttoffangle[4];

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
GLdouble headAngle = 180.0f;
GLdouble wheelRollAngle = 0.0f;
GLdouble wheelRollRate = 0.0f;
GLdouble policeLightAngle = 0.0f;
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
bool copLightsOn = false;

#define CAR_POINT_COUNT 72
vec4 carVerts[CAR_POINT_COUNT];
vec3 carNormals[CAR_POINT_COUNT];
vec4 carColors[CAR_POINT_COUNT];
vec4 carAmbient[CAR_POINT_COUNT];
vec4 carDiffuse[CAR_POINT_COUNT];
vec4 carSpecular[CAR_POINT_COUNT];
#define STAGE_POINT_COUNT 36
vec4 stageVerts[STAGE_POINT_COUNT];
vec3 stageNormals[STAGE_POINT_COUNT];
vec4 stageColors[STAGE_POINT_COUNT];
vec4 stageAmbient[STAGE_POINT_COUNT];
vec4 stageDiffuse[STAGE_POINT_COUNT];
vec4 stageSpecular[STAGE_POINT_COUNT];
#define WHEEL_POINT_COUNT 362
vec4 wheelVerts[WHEEL_POINT_COUNT*2];
vec3 wheelNormals[WHEEL_POINT_COUNT*2];
vec4 wheelColors[WHEEL_POINT_COUNT*2];
vec4 wheelAmbient[WHEEL_POINT_COUNT*2];
vec4 wheelDiffuse[WHEEL_POINT_COUNT*2];
vec4 wheelSpecular[WHEEL_POINT_COUNT*2];
#define WHEEL_STRIPE_POINT_COUNT 3
vec4 wheelStripeVerts[WHEEL_STRIPE_POINT_COUNT];
vec3 wheelStripeNormals[WHEEL_STRIPE_POINT_COUNT];
vec4 wheelStripeColors[WHEEL_STRIPE_POINT_COUNT];
vec4 wheelStripeAmbient[WHEEL_STRIPE_POINT_COUNT];
vec4 wheelStripeDiffuse[WHEEL_STRIPE_POINT_COUNT];
vec4 wheelStripeSpecular[WHEEL_STRIPE_POINT_COUNT];
#define WHEEL_CONNECTOR_POINT_COUNT (360*6)+6
vec4 wheelConVerts[WHEEL_CONNECTOR_POINT_COUNT];
vec3 wheelConNormals[WHEEL_CONNECTOR_POINT_COUNT];
vec4 wheelConColors[WHEEL_CONNECTOR_POINT_COUNT];
vec4 wheelConAmbient[WHEEL_CONNECTOR_POINT_COUNT];
vec4 wheelConDiffuse[WHEEL_CONNECTOR_POINT_COUNT];
vec4 wheelConSpecular[WHEEL_CONNECTOR_POINT_COUNT];
int headVertCount;
vec4* headVerts;
vec3* headNormals;
vec4 *headColors;
vec4* headAmbient;
vec4* headDiffuse;
vec4* headSpecular;
int eyeVertCount;
vec4* eyeVerts;
vec3* eyeNormals;
vec4* eyeColors;
vec4* eyeAmbient;
vec4* eyeDiffuse;
vec4* eyeSpecular;
#define PYLON_POINT_COUNT 36
vec4 pylonVerts[4][PYLON_POINT_COUNT];
vec3 pylonNormals[4][PYLON_POINT_COUNT];
vec4 pylonColors[4][PYLON_POINT_COUNT];
vec4 pylonAmbient[4][PYLON_POINT_COUNT];
vec4 pylonDiffuse[4][PYLON_POINT_COUNT];
vec4 pylonSpecular[4][PYLON_POINT_COUNT];

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

#define PYLON_WIDTH 1
#define PYLON_DEPTH 1
#define PYLON_HEIGHT 5

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

	for (int i = 0; i < 36; i++)
	{
		float ambientFactor = 0.8;
		float diffuseFactor = 1.0;
		float specularFactor = 1.0;
		carAmbient[i] = vec4(carColors[i].x * ambientFactor, carColors[i].y * ambientFactor, carColors[i].z * ambientFactor, carColors[i].w);
		carDiffuse[i] = vec4(carColors[i].x * diffuseFactor, carColors[i].y * diffuseFactor, carColors[i].z * diffuseFactor, carColors[i].w);
		carSpecular[i] = vec4(carColors[i].x * specularFactor, carColors[i].y * specularFactor, carColors[i].z * specularFactor, carColors[i].w);
	}

	for(int i=36; i<72; i++){
		carColors[i] = vec4(1.0, 1.0, 1.0, 1.0);

		float ambientFactor = 0.8;
		float diffuseFactor = 1.0;
		float specularFactor = 1.0;
		carAmbient[i] = vec4(carColors[i].x * ambientFactor, carColors[i].y * ambientFactor, carColors[i].z * ambientFactor, carColors[i].w);
		carDiffuse[i] = vec4(carColors[i].x * diffuseFactor, carColors[i].y * diffuseFactor, carColors[i].z * diffuseFactor, carColors[i].w);
		carSpecular[i] = vec4(carColors[i].x * specularFactor, carColors[i].y * specularFactor, carColors[i].z * specularFactor, carColors[i].w);

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

	for (int j = 0; j < i; j++)
	{
		carNormals[j] = vec3(carVerts[j].x, carVerts[j].y, carVerts[j].z);
		
	}
}
void generateStage()
{
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

	for (int i = 0; i < 36; i++)
	{
		stageColors[i] = vec4(0.25, 0.25, 0.25, 1.0);
		stageNormals[i] = vec3(stageVerts[i].x, stageVerts[i].y, stageVerts[i].z);
		
		float ambientFactor = 1.0;
		float diffuseFactor = 4.0;
		float specularFactor = 0.2;
		stageAmbient[i] = vec4(stageColors[i].x * ambientFactor, stageColors[i].y * ambientFactor, stageColors[i].z * ambientFactor, stageColors[i].w);
		stageDiffuse[i] = vec4(stageColors[i].x * diffuseFactor, stageColors[i].y * diffuseFactor, stageColors[i].z * diffuseFactor, stageColors[i].w);
		stageSpecular[i] = vec4(stageColors[i].x * specularFactor, stageColors[i].y * specularFactor, stageColors[i].z * specularFactor, stageColors[i].w);
	}
}
void generateWheel()
{
	float wheel_ambient, wheel_diffuse, wheel_specular;
	wheel_ambient = 0.3;
	wheel_diffuse = 0.1;
	wheel_specular = 0.9;

	int i;
	for (i = 0; i < WHEEL_POINT_COUNT; i++)
	{
		wheelColors[i] = vec4(0.75, 0.75, 0.75, 1.0);

		float ambientFactor = 0.8;
		float diffuseFactor = 1.0;
		float specularFactor = 1.0;
		wheelAmbient[i] = vec4(wheelColors[i].x * ambientFactor, wheelColors[i].y * ambientFactor, wheelColors[i].z * ambientFactor, wheelColors[i].w);
		wheelDiffuse[i] = vec4(wheelColors[i].x * diffuseFactor, wheelColors[i].y * diffuseFactor, wheelColors[i].z * diffuseFactor, wheelColors[i].w);
		wheelSpecular[i] = vec4(wheelColors[i].x * specularFactor, wheelColors[i].y * specularFactor, wheelColors[i].z * specularFactor, wheelColors[i].w);
	}
	for ( ; i < WHEEL_POINT_COUNT*2; i++)
	{
		wheelColors[i] = vec4(0.75, 0.75, 0.75, 1.0);
	}

	wheelVerts[0] = vec4(0.0, 0.0, WHEEL_THICKNESS, 1.0);
	wheelNormals[0] = vec3(wheelVerts[0].x, wheelVerts[0].y, wheelVerts[0].z);
	wheelVerts[0+WHEEL_POINT_COUNT] = vec4(0.0, 0.0, -WHEEL_THICKNESS, 1.0);
	wheelNormals[0+WHEEL_POINT_COUNT] = vec3(wheelVerts[0+WHEEL_POINT_COUNT].x, wheelVerts[0+WHEEL_POINT_COUNT].y, wheelVerts[0+WHEEL_POINT_COUNT].z);

	int connectorCount = 0;
	for ( i = 1; i < WHEEL_POINT_COUNT; i++)
	{
		float Angle = (i-1) * (2.0*M_PI/180);
		float X = cos(Angle)*WHEEL_RADIUS;
		float Y = sin(Angle)*WHEEL_RADIUS;
		wheelVerts[i] = vec4(X, Y, 0.5, 1.0);
		wheelNormals[i] = vec3(wheelVerts[i].x, wheelVerts[i].y, wheelVerts[i].z);
		wheelVerts[i+WHEEL_POINT_COUNT] = vec4(X, Y, -WHEEL_THICKNESS, 1.0);
		wheelNormals[i+WHEEL_POINT_COUNT] = vec3(wheelVerts[i+WHEEL_POINT_COUNT].x, wheelVerts[i+WHEEL_POINT_COUNT].y, wheelVerts[i+WHEEL_POINT_COUNT].z);
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
		wheelConNormals[i] = vec3(wheelConVerts[i].x, wheelConVerts[i].y, wheelConVerts[i].z);
		
		float ambientFactor = 1.0;
		float diffuseFactor = 2.0;
		float specularFactor = 1.0;
		wheelConAmbient[i] = vec4(wheelConColors[i].x * ambientFactor, wheelConColors[i].y * ambientFactor, wheelConColors[i].z * ambientFactor, wheelConColors[i].w);
		wheelConDiffuse[i] = vec4(wheelConColors[i].x * diffuseFactor, wheelConColors[i].y * diffuseFactor, wheelConColors[i].z * diffuseFactor, wheelConColors[i].w);
		wheelConSpecular[i] = vec4(wheelConColors[i].x * specularFactor, wheelConColors[i].y * specularFactor, wheelConColors[i].z * specularFactor, wheelConColors[i].w);
	}


	wheelStripeVerts[0] = vec4(-WHEEL_RADIUS*.75, -WHEEL_RADIUS*.75, WHEEL_THICKNESS+0.001, 1.0);
	wheelStripeVerts[1] = vec4(WHEEL_RADIUS*.75, -WHEEL_RADIUS*.75, WHEEL_THICKNESS+0.001, 1.0);
	wheelStripeVerts[2] = vec4(0.0, WHEEL_RADIUS*.75, WHEEL_THICKNESS+0.001, 1.0);
	wheelStripeNormals[0] = vec3(-WHEEL_RADIUS*.75, -WHEEL_RADIUS*.75, WHEEL_THICKNESS+0.001);
	wheelStripeNormals[1] = vec3(WHEEL_RADIUS*.75, -WHEEL_RADIUS*.75, WHEEL_THICKNESS+0.001);
	wheelStripeNormals[2] = vec3(0.0, WHEEL_RADIUS*.75, WHEEL_THICKNESS+0.001);
	wheelStripeColors[0] = vec4(1.0, 0.0, 0.0, 1.0);
	wheelStripeColors[1] = vec4(0.0, 1.0, 0.0, 1.0);
	wheelStripeColors[2] = vec4(0.0, 0.0, 1.0, 1.0);

	for (int i = 0; i<3; i++)
	{
		float ambientFactor = 0.8;
		float diffuseFactor = 1.0;
		float specularFactor = 1.0;
		wheelStripeAmbient[i] = vec4(wheelStripeColors[i].x * ambientFactor, wheelStripeColors[i].y * ambientFactor, wheelStripeColors[i].z * ambientFactor, wheelStripeColors[i].w);
		wheelStripeDiffuse[i] = vec4(wheelStripeColors[i].x * diffuseFactor, wheelStripeColors[i].y * diffuseFactor, wheelStripeColors[i].z * diffuseFactor, wheelStripeColors[i].w);
		wheelStripeSpecular[i] = vec4(wheelStripeColors[i].x * specularFactor, wheelStripeColors[i].y * specularFactor, wheelStripeColors[i].z * specularFactor, wheelStripeColors[i].w);
	}
}
void generateHead()
{
	float radius = HEAD_RADIUS;
	int subdiv = 100;
	float step = (360.0/subdiv)*(M_PI/180.0);

	headVertCount = ceil(subdiv/2.0)*subdiv * 6;

	if(headVerts){
		delete[] headVerts;
	}
	headVerts = new vec4[headVertCount];

	int k = 0;
	for(float i = -M_PI/2; i<=M_PI/2; i+=step){
		for(float j = -M_PI; j<=M_PI; j+=step){
			//triangle 1
			headVerts[k++]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
	
			headVerts[k++]=   vec4(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step), 1.0);
			
			headVerts[k++]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			
			//triangle 2
			headVerts[k++]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			
			headVerts[k++]=   vec4(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i), 1.0);
			
			headVerts[k++]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
		}
	}

	if (headColors){
		delete[] headColors;
	}
	headColors = new vec4[headVertCount];
	if (headNormals){
		delete[] headNormals;
	}
	headNormals = new vec3[headVertCount];
	if (headAmbient){
		delete[] headAmbient;
	}
	headAmbient= new vec4[headVertCount];
	if (headDiffuse){
		delete[] headDiffuse;
	}
	headDiffuse = new vec4[headVertCount];
	if (headSpecular){
		delete[] headSpecular;
	}
	headSpecular = new vec4[headVertCount];

	for (int i = 0; i < headVertCount; i++)
	{
		headColors[i] = vec4(1.0, 1.0, 1.0, 1.0);
		headNormals[i] = vec3(headVerts[i].x, headVerts[i].y, headVerts[i].z);
		float ambientFactor = 1.0;
		float diffuseFactor = 1.0;
		float specularFactor = 1.0;
		headAmbient[i] = vec4(headColors[i].x * ambientFactor, headColors[i].y * ambientFactor, headColors[i].z * ambientFactor, headColors[i].w);
		headDiffuse[i] = vec4(headColors[i].x * diffuseFactor, headColors[i].y * diffuseFactor, headColors[i].z * diffuseFactor, headColors[i].w);
		headSpecular[i] = vec4(headColors[i].x * specularFactor, headColors[i].y * specularFactor, headColors[i].z * specularFactor, headColors[i].w);
	}
}
void generateEye()
{
	float radius = EYE_RADIUS;
	int subdiv = 10;
	float step = (360.0/subdiv)*(M_PI/180.0);

	eyeVertCount = ceil(subdiv/2.0)*subdiv * 6;

	if(eyeVerts){
		delete[] eyeVerts;
	}
	eyeVerts = new vec4[eyeVertCount];

	int k = 0;
	for(float i = -M_PI/2; i<=M_PI/2; i+=step){
		for(float j = -M_PI; j<=M_PI; j+=step){
			//triangle 1
			eyeVerts[k++]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			
			eyeVerts[k++]=   vec4(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step), 1.0);
			
			eyeVerts[k++]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			
			//triangle 2
			eyeVerts[k++]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);

			eyeVerts[k++]=   vec4(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i), 1.0);

			eyeVerts[k++]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
		}
	}

	if (eyeColors){
		delete[] eyeColors;
	}
	eyeColors = new vec4[eyeVertCount];

	if (eyeNormals){
		delete[] eyeNormals;
	}
	eyeNormals = new vec3[eyeVertCount];
	
	if (eyeAmbient){
		delete[] eyeAmbient;
	}
	eyeAmbient = new vec4[eyeVertCount];

	if (eyeDiffuse){
		delete[] eyeDiffuse;
	}
	eyeDiffuse = new vec4[eyeVertCount];

	if (eyeSpecular){
		delete[] eyeSpecular;
	}
	eyeSpecular = new vec4[eyeVertCount];

	for (int i = 0; i < eyeVertCount; i++)
	{
		eyeColors[i] = vec4(0.0, 0.0, 0.0, 1.0);
		eyeNormals[i] = vec3(eyeVerts[i].x, eyeVerts[i].y, eyeVerts[i].z);
		float ambientFactor = 0.2;
		float diffuseFactor = 0.2;
		float specularFactor = 1.0;
		eyeAmbient[i] = vec4(eyeColors[i].x * ambientFactor, eyeColors[i].y * ambientFactor, eyeColors[i].z * ambientFactor, eyeColors[i].w);
		eyeDiffuse[i] = vec4(eyeColors[i].x * diffuseFactor, eyeColors[i].y * diffuseFactor, eyeColors[i].z * diffuseFactor, eyeColors[i].w);
		eyeSpecular[i] = vec4(eyeColors[i].x * specularFactor, eyeColors[i].y * specularFactor, eyeColors[i].z * specularFactor, eyeColors[i].w);
	}
}
void generatePylon()
{
	for (int i = 0; i < 4; i++)
	{
		pylonVerts[i][0] = vec4(PYLON_WIDTH, 0, PYLON_DEPTH, 1.0);
		pylonVerts[i][1] = vec4(PYLON_WIDTH, PYLON_HEIGHT, PYLON_DEPTH, 1.0);
		pylonVerts[i][2] = vec4(-PYLON_WIDTH, PYLON_HEIGHT, PYLON_DEPTH, 1.0);
		pylonVerts[i][3] = vec4(-PYLON_WIDTH, PYLON_HEIGHT, PYLON_DEPTH, 1.0);
		pylonVerts[i][4] = vec4(-PYLON_WIDTH, 0, PYLON_DEPTH, 1.0);
		pylonVerts[i][5] = vec4(PYLON_WIDTH, 0, PYLON_DEPTH, 1.0);
		pylonVerts[i][6] = vec4(-PYLON_WIDTH, 0, -PYLON_DEPTH, 1.0);
		pylonVerts[i][7] = vec4(-PYLON_WIDTH, PYLON_HEIGHT, -PYLON_DEPTH, 1.0);
		pylonVerts[i][8] = vec4(PYLON_WIDTH, PYLON_HEIGHT, -PYLON_DEPTH, 1.0);
		pylonVerts[i][9] = vec4(PYLON_WIDTH, PYLON_HEIGHT, -PYLON_DEPTH, 1.0);
		pylonVerts[i][10] = vec4(PYLON_WIDTH, 0, -PYLON_DEPTH, 1.0);
		pylonVerts[i][11] = vec4(-PYLON_WIDTH, 0, -PYLON_DEPTH, 1.0);
		pylonVerts[i][12] = vec4(PYLON_WIDTH, PYLON_HEIGHT, PYLON_DEPTH, 1.0);
		pylonVerts[i][13] = vec4(PYLON_WIDTH, 0, PYLON_DEPTH, 1.0);
		pylonVerts[i][14] = vec4(PYLON_WIDTH, 0, -PYLON_DEPTH, 1.0);
		pylonVerts[i][15] = vec4(PYLON_WIDTH, 0, -PYLON_DEPTH, 1.0);
		pylonVerts[i][16] = vec4(PYLON_WIDTH, PYLON_HEIGHT, -PYLON_DEPTH, 1.0);
		pylonVerts[i][17] = vec4(PYLON_WIDTH, PYLON_HEIGHT, PYLON_DEPTH, 1.0);
		pylonVerts[i][18] = vec4(-PYLON_WIDTH, PYLON_HEIGHT, -PYLON_DEPTH, 1.0);
		pylonVerts[i][19] = vec4(-PYLON_WIDTH, 0, -PYLON_DEPTH, 1.0);
		pylonVerts[i][20] = vec4(-PYLON_WIDTH, 0, PYLON_DEPTH, 1.0);
		pylonVerts[i][21] = vec4(-PYLON_WIDTH, 0, PYLON_DEPTH, 1.0);
		pylonVerts[i][22] = vec4(-PYLON_WIDTH, PYLON_HEIGHT, PYLON_DEPTH, 1.0);
		pylonVerts[i][23] = vec4(-PYLON_WIDTH, PYLON_HEIGHT, -PYLON_DEPTH, 1.0);
		pylonVerts[i][24] = vec4(PYLON_WIDTH, PYLON_HEIGHT, PYLON_DEPTH, 1.0);
		pylonVerts[i][25] = vec4(PYLON_WIDTH, PYLON_HEIGHT, -PYLON_DEPTH, 1.0);
		pylonVerts[i][26] = vec4(-PYLON_WIDTH, PYLON_HEIGHT, -PYLON_DEPTH, 1.0);
		pylonVerts[i][27] = vec4(-PYLON_WIDTH, PYLON_HEIGHT, -PYLON_DEPTH, 1.0);
		pylonVerts[i][28] = vec4(-PYLON_WIDTH, PYLON_HEIGHT, PYLON_DEPTH, 1.0);
		pylonVerts[i][29] = vec4(PYLON_WIDTH, PYLON_HEIGHT, PYLON_DEPTH, 1.0);
		pylonVerts[i][30] = vec4(PYLON_WIDTH, 0, -PYLON_DEPTH, 1.0);
		pylonVerts[i][31] = vec4(PYLON_WIDTH, 0, PYLON_DEPTH, 1.0);
		pylonVerts[i][32] = vec4(-PYLON_WIDTH, 0, PYLON_DEPTH, 1.0);
		pylonVerts[i][33] = vec4(-PYLON_WIDTH, 0, PYLON_DEPTH, 1.0);
		pylonVerts[i][34] = vec4(-PYLON_WIDTH, 0, -PYLON_DEPTH, 1.0);
		pylonVerts[i][35] = vec4(PYLON_WIDTH, 0, -PYLON_DEPTH, 1.0);
	}

	for(int i=0; i<36; i++){
		pylonColors[0][i] = vec4(1.0, 0.0, 0.0, 1.0);
		pylonColors[1][i] = vec4(0.0, 1.0, 0.0, 1.0);
		pylonColors[2][i] = vec4(0.0, 0.0, 1.0, 1.0);
		pylonColors[3][i] = vec4(0.0, 1.0, 1.0, 1.0);

		for (int j=0; j < 4; j++)
		{
			pylonNormals[j][i] = vec3(pylonVerts[j][i].x, pylonVerts[j][i].y, pylonVerts[j][i].z);

			float ambientFactor = 0.8;
			float diffuseFactor = 1.0;
			float specularFactor = 1.0;
			pylonAmbient[j][i] = vec4(pylonColors[j][i].x * ambientFactor, pylonColors[j][i].y * ambientFactor, pylonColors[j][i].z * ambientFactor, pylonColors[j][i].w);
			pylonDiffuse[j][i] = vec4(pylonColors[j][i].x * diffuseFactor, pylonColors[j][i].y * diffuseFactor, pylonColors[j][i].z * diffuseFactor, pylonColors[j][i].w);
			pylonSpecular[j][i] = vec4(pylonColors[j][i].x * specularFactor, pylonColors[j][i].y * specularFactor, pylonColors[j][i].z * specularFactor, pylonColors[j][i].w);
		}
	}

}
/* Since the wheel is kind of complex, use this helper function to draw it 
after the draw position has been set up. */
void drawWheel(void)
{
	glBindVertexArray( vao[WHEEL] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, WHEEL_POINT_COUNT );    // draw the outside wheel 
	glDrawArrays( GL_TRIANGLE_FAN, WHEEL_POINT_COUNT, WHEEL_POINT_COUNT );    // draw the inside wheel 
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
			//mv = LookAt(vec4(0, 20, 0, 1.0), vec4(0, 0, 0, 1.0), vec4(1, 0, 0, 0.0));
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
		float Angle = (M_PI*(carAngle)/180);
		float X = sin(Angle)*20;
		float Z = cos(Angle)*20;
		mv = LookAt(vec4(xPosition+X, 20, zPosition+Z, 1.0), 
			vec4(xPosition-X, 1, zPosition-Z, 1.0), vec4(0, 1, 0, 0.0));
	}

	mv = mv*Translate(tx, ty, tz);
	mv = mv*RotateX(rx);
	mv = mv*RotateY(ry);
	mv = mv*RotateZ(rz);
	mv = mv*Scale(sx, sy, sz);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	mat4 original = mv;

	// and we also need to send our projection matrix, which again is more appropriately
	// a uniform instead of an attribute since it's the same for every vertex
	if (camera == STATIC_CAMERA)
	{
		p = Perspective(zoom, (float)ww/(float)wh, 1.0, 100.0);
	}
	else if (camera == VIEWPOINT_CAMERA)
	{
		p = Perspective(45, (float)ww/(float)wh, (HEAD_RADIUS*2+(EYE_RADIUS*2)), 100.0);
	}
	else if (camera == CHASE_CAMERA)
	{
		p = Perspective(45, (float)ww/(float)wh, 1.0, 100.0);
	}

	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	// Setup the ambient light
	glUniform4fv(ambient_light, 1, vec4(0.3, 0.3, 0.3, 1));
	//glVertexAttrib4fv(vAmbientDiffuseColor, vec4(0, 0.5, 0, 1));

		float Angle = (M_PI*(carAngle)/180);
		float X = sin(Angle);
		float Z = cos(Angle);

	// Confirmed light_position is placed with the next block of code
	//mv = mv*Translate(-X*CAR_LENGTH, 0, -Z*CAR_LENGTH);

	mv = mv*Translate(0.0, CAR_HEIGHT+WHEEL_RADIUS, 0.0);
	mv = mv*Translate(xPosition, yPosition, zPosition);
	mv = mv*RotateY(carAngle);
	mat4 carCenter = mv;
	mv = mv*Translate(-0.75*CAR_WIDTH, 0, -CAR_LENGTH);
	mat4 lights[4];
	lights[0] = mv;

	vec4 lightPos[4];
	vec4 lightVector[4];
	vec4 lightColor[4];

	// Left Headlight
	lightPos[0] = mv*vec4(0, 0, 0, 1.0);
	lightVector[0] = mv*vec4(0,-10,-20, 1.0);
	lightVector[0] = lightVector[0]*-1;
	lightColor[0] = vec4(1.0, 1.0, 1.0, 1.0);
	
	// Right Headlight
	mv = mv*Translate(0.75*CAR_WIDTH*2, 0, 0);
	lightPos[1] = mv*vec4(0, 0, 0, 1.0);
	lights[1] = mv;
	lightVector[1] = mv*vec4(0,-10,-20, 1.0);
	lightVector[1] = lightVector[1]*-1;
	lightColor[1] = vec4(1.0, 1.0, 1.0, 1.0);

	// Red Police Light
	mv = carCenter;
	mv = mv*Translate(-CAR_WIDTH, CAR_HEIGHT, 0);
	mv = mv*RotateY(policeLightAngle);
	lightPos[2] = mv*vec4(0, 0, 0, 1.0);
	lights[2] = mv;
	lightVector[2] = mv*vec4(-20,-10,0, 1.0);
	lightVector[2] = lightVector[2]*-1;

	// Blue Police Light
	mv = carCenter;
	mv = mv*Translate(CAR_WIDTH, CAR_HEIGHT, 0);
	mv = mv*RotateY(-policeLightAngle);
	lightPos[3] = mv*vec4(0, 0, 0, 1.0);
	lights[3] = mv;
	lightVector[3] = mv*vec4(20,-10, 0, 1.0);
	lightVector[3] = lightVector[3]*-1;

		
	if (copLightsOn)
	{
		lightColor[2] = vec4(1.0, 0.0, 0.0, 1.0);
		lightColor[3] = vec4(0.0, 0.0, 1.0, 1.0);
	}
	else
	{
		lightColor[2] = vec4(0.0, 0.0, 0.0, 1.0);
		lightColor[3] = vec4(0.0, 0.0, 0.0, 1.0);
	}

	float tmp[4*4];

	for (int i = 0; i < 4; i++)
	{
		tmp[(i*4)] = lightPos[i].x;
		tmp[(i*4)+1] = lightPos[i].y;
		tmp[(i*4)+2] = lightPos[i].z;
		tmp[(i*4)+3] = lightPos[i].w;
	}
	glUniform4fv(light_position[0], 4, tmp);

	for (int i = 0; i < 4; i++)
	{
		tmp[(i*4)] = lightColor[i].x;
		tmp[(i*4)+1] = lightColor[i].y;
		tmp[(i*4)+2] = lightColor[i].z;
		tmp[(i*4)+3] = lightColor[i].w;
	}
	glUniform4fv(light_color[0], 4, tmp);

	for (int i = 0; i < 4; i++)
	{
		tmp[(i*4)] = lightVector[i].x;
		tmp[(i*4)+1] = lightVector[i].y;
		tmp[(i*4)+2] = lightVector[i].z;
		tmp[(i*4)+3] = lightVector[i].w;
	}
	glUniform4fv(light_direction[0], 4, tmp);

	float cutoffangle[4] = {(M_PI*20)/180, (M_PI*20)/180, (M_PI*90)/180, (M_PI*90)/180};
	glUniform1fv(light_cuttoffangle[0], 4, cutoffangle);

	/* Debug to draw a white ball where each light position is at */ 
	for (int i = 0; i < 4; i++)
	{
		glUniformMatrix4fv(model_view, 1, GL_TRUE, lights[i]);
		glBindVertexArray( vao[HEAD] );
		glDrawArrays(GL_TRIANGLES, 0, headVertCount);
	}

	mv = original;
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[STAGE] );
	glDrawArrays( GL_TRIANGLES, 0, STAGE_POINT_COUNT );    // draw the stage

	mv = mv*Translate(-20, 0, -20);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[PYLON] );
	glDrawArrays( GL_TRIANGLES, 0, PYLON_POINT_COUNT );    // draw the pylons

	mv = original;
	mv = mv*Translate(20, 0, -20);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[PYLON] );
	glDrawArrays( GL_TRIANGLES, PYLON_POINT_COUNT, PYLON_POINT_COUNT*2 );

	mv = original;
	mv = mv*Translate(20, 0, 20);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[PYLON] );
	glDrawArrays( GL_TRIANGLES, PYLON_POINT_COUNT*2, PYLON_POINT_COUNT*3 );

	mv = original;
	mv = mv*Translate(-20, 0, 20);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[PYLON] );
	glDrawArrays( GL_TRIANGLES, PYLON_POINT_COUNT*3, PYLON_POINT_COUNT*4 );

	mv = original;
	mv = mv*Translate(0.0, CAR_HEIGHT+(WHEEL_RADIUS), 0.0);
	mv = mv*Translate(xPosition, yPosition, zPosition);
	mv = mv*RotateY(carAngle);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	
	glBindVertexArray( vao[CAR] );
	glDrawArrays( GL_TRIANGLES, 0, CAR_POINT_COUNT );    // draw the car 

	original = mv;

	mv = mv*Translate(0.0, CAR_HEIGHT+(HEAD_RADIUS*2), 0.0);
	mv = mv*RotateY(headAngle);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[HEAD] );
	glDrawArrays(GL_TRIANGLES, 0, headVertCount); // draw the head 
	
	mat4 headOriginal = mv;
	mv = mv*Translate(.4*HEAD_RADIUS, 0.0, HEAD_RADIUS);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[EYE] );
	glDrawArrays(GL_TRIANGLES, 0, eyeVertCount);    // draw one eye 
	mv = headOriginal;
	mv = mv*Translate(-.4*HEAD_RADIUS, 0.0, HEAD_RADIUS);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[EYE] );
	glDrawArrays(GL_TRIANGLES, 0, eyeVertCount);    // draw the other eye 

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
		headAngle += 2.0;
	}
	if (key == 'x')
	{
		headAngle -= 2.0;
	}
	if (key == ' ')
	{
		stopCar();
	}

	if (key == 'l')
	{
		copLightsOn = !copLightsOn;
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
		staticCameraCenterOfStage = true;
		camera = STATIC_CAMERA;
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
	generatePylon();

	// Load shaders and use the resulting shader program
	GLuint program = InitShader( "vshader-transform.glsl", "fshader-transform.glsl" );
	glUseProgram( program );

	// Create all vertex array object
	glGenVertexArrays( VAO_COUNT, &vao[0] );
	glGenBuffers(VAO_COUNT*6, &vbo[0]);

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
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carNormals), carNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carAmbient), carAmbient, GL_STATIC_DRAW );
	vAmbient = glGetAttribLocation(program, "vAmbient");
	glEnableVertexAttribArray(vAmbient);
	glVertexAttribPointer(vAmbient, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carDiffuse), carDiffuse, GL_STATIC_DRAW );
	vDiffuse = glGetAttribLocation(program, "vDiffuse");
	glEnableVertexAttribArray(vDiffuse);
	glVertexAttribPointer(vDiffuse, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carSpecular), carSpecular, GL_STATIC_DRAW );
	vSpecular = glGetAttribLocation(program, "vSpecular");
	glEnableVertexAttribArray(vSpecular);
	glVertexAttribPointer(vSpecular, 4, GL_FLOAT, GL_FALSE, 0, 0);

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
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(stageNormals), stageNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(stageAmbient), stageAmbient, GL_STATIC_DRAW );
	vAmbient = glGetAttribLocation(program, "vAmbient");
	glEnableVertexAttribArray(vAmbient);
	glVertexAttribPointer(vAmbient, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(stageDiffuse), stageDiffuse, GL_STATIC_DRAW );
	vDiffuse = glGetAttribLocation(program, "vDiffuse");
	glEnableVertexAttribArray(vDiffuse);
	glVertexAttribPointer(vDiffuse, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(stageSpecular), stageSpecular, GL_STATIC_DRAW );
	vSpecular = glGetAttribLocation(program, "vSpecular");
	glEnableVertexAttribArray(vSpecular);
	glVertexAttribPointer(vSpecular, 4, GL_FLOAT, GL_FALSE, 0, 0);

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
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelNormals), wheelNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelAmbient), wheelAmbient, GL_STATIC_DRAW );
	vAmbient = glGetAttribLocation(program, "vAmbient");
	glEnableVertexAttribArray(vAmbient);
	glVertexAttribPointer(vAmbient, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelDiffuse), wheelDiffuse, GL_STATIC_DRAW );
	vDiffuse = glGetAttribLocation(program, "vDiffuse");
	glEnableVertexAttribArray(vDiffuse);
	glVertexAttribPointer(vDiffuse, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelSpecular), wheelSpecular, GL_STATIC_DRAW );
	vSpecular = glGetAttribLocation(program, "vSpecular");
	glEnableVertexAttribArray(vSpecular);
	glVertexAttribPointer(vSpecular, 4, GL_FLOAT, GL_FALSE, 0, 0);


	glBindVertexArray( vao[HEAD] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, headVertCount*sizeof(vec4), headVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, headVertCount*sizeof(vec4), headColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, headVertCount*sizeof(vec3), headNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(headAmbient), headAmbient, GL_STATIC_DRAW );
	vAmbient = glGetAttribLocation(program, "vAmbient");
	glEnableVertexAttribArray(vAmbient);
	glVertexAttribPointer(vAmbient, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(headDiffuse), headDiffuse, GL_STATIC_DRAW );
	vDiffuse = glGetAttribLocation(program, "vDiffuse");
	glEnableVertexAttribArray(vDiffuse);
	glVertexAttribPointer(vDiffuse, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(headSpecular), headSpecular, GL_STATIC_DRAW );
	vSpecular = glGetAttribLocation(program, "vSpecular");
	glEnableVertexAttribArray(vSpecular);
	glVertexAttribPointer(vSpecular, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray( vao[EYE] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, eyeVertCount*sizeof(vec4), eyeVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, eyeVertCount*sizeof(vec4), eyeColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, eyeVertCount*sizeof(vec3), eyeNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(eyeAmbient), eyeAmbient, GL_STATIC_DRAW );
	vAmbient = glGetAttribLocation(program, "vAmbient");
	glEnableVertexAttribArray(vAmbient);
	glVertexAttribPointer(vAmbient, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(eyeDiffuse), eyeDiffuse, GL_STATIC_DRAW );
	vDiffuse = glGetAttribLocation(program, "vDiffuse");
	glEnableVertexAttribArray(vDiffuse);
	glVertexAttribPointer(vDiffuse, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(eyeSpecular), eyeSpecular, GL_STATIC_DRAW );
	vSpecular = glGetAttribLocation(program, "vSpecular");
	glEnableVertexAttribArray(vSpecular);
	glVertexAttribPointer(vSpecular, 4, GL_FLOAT, GL_FALSE, 0, 0);

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
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelStripeNormals), wheelStripeNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelStripeAmbient), wheelStripeAmbient, GL_STATIC_DRAW );
	vAmbient = glGetAttribLocation(program, "vAmbient");
	glEnableVertexAttribArray(vAmbient);
	glVertexAttribPointer(vAmbient, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelStripeDiffuse), wheelStripeDiffuse, GL_STATIC_DRAW );
	vDiffuse = glGetAttribLocation(program, "vDiffuse");
	glEnableVertexAttribArray(vDiffuse);
	glVertexAttribPointer(vDiffuse, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelStripeSpecular), wheelStripeSpecular, GL_STATIC_DRAW );
	vSpecular = glGetAttribLocation(program, "vSpecular");
	glEnableVertexAttribArray(vSpecular);
	glVertexAttribPointer(vSpecular, 4, GL_FLOAT, GL_FALSE, 0, 0);

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
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelNormals), wheelNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelAmbient), wheelAmbient, GL_STATIC_DRAW );
	vAmbient = glGetAttribLocation(program, "vAmbient");
	glEnableVertexAttribArray(vAmbient);
	glVertexAttribPointer(vAmbient, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelDiffuse), wheelDiffuse, GL_STATIC_DRAW );
	vDiffuse = glGetAttribLocation(program, "vDiffuse");
	glEnableVertexAttribArray(vDiffuse);
	glVertexAttribPointer(vDiffuse, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(wheelSpecular), wheelSpecular, GL_STATIC_DRAW );
	vSpecular = glGetAttribLocation(program, "vSpecular");
	glEnableVertexAttribArray(vSpecular);
	glVertexAttribPointer(vSpecular, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray( vao[PYLON] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(pylonVerts), pylonVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(pylonColors), pylonColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(pylonNormals), pylonNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(pylonAmbient), pylonAmbient, GL_STATIC_DRAW );
	vAmbient = glGetAttribLocation(program, "vAmbient");
	glEnableVertexAttribArray(vAmbient);
	glVertexAttribPointer(vAmbient, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(pylonDiffuse), pylonDiffuse, GL_STATIC_DRAW );
	vDiffuse = glGetAttribLocation(program, "vDiffuse");
	glEnableVertexAttribArray(vDiffuse);
	glVertexAttribPointer(vDiffuse, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, vbo[vboIndex++] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(pylonSpecular), pylonSpecular, GL_STATIC_DRAW );
	vSpecular = glGetAttribLocation(program, "vSpecular");
	glEnableVertexAttribArray(vSpecular);
	glVertexAttribPointer(vSpecular, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//grab pointers for our modelview and perspecive uniform matrices
	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	ambient_light = glGetUniformLocation(program, "ambient_light");
	vAmbientDiffuseColor = glGetAttribLocation(program, "vAmbientDiffuseColor");

	for (int i = 0; i < 4; i++)
	{
		light_position[i] = glGetUniformLocation(program, "light_position");
		light_color[i] = glGetUniformLocation(program, "light_color");
		light_direction[i] = glGetUniformLocation(program, "light_direction");
		light_cuttoffangle[i] = glGetUniformLocation(program, "light_cutoffangle");
	}

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

	if (copLightsOn)
	{
		policeLightAngle += 5.0;
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
	glutCreateWindow("Assignment 4 - Ross Anderson");  

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