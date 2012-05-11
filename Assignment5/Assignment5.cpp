/*
 * Assignment 5 - Earth Texture Mapping
 *
 * Continued on from: Skeleton lighting program
 * SEIS750
 * Spring 2012
 * by Ross Anderson
 **/

#include <GL/Angel.h>
#include <math.h>
#include <IL/il.h>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib,"ILUT.lib")
#pragma comment(lib,"DevIL.lib")
#pragma comment(lib,"ILU.lib")

//store window width and height
int ww=700, wh=700;

#define M_PI 3.14159265358979323846

//If you want more than one type of shader, store references to your shader program objects
GLuint activeProgram, colorMapProgram, specularMapProgram, allFeaturesMapProgram, cloudMapProgram;

typedef enum
{
	DAY_TEXTURE,
	NIGHT_TEXTURE,
	SPECULAR_TEXTURE,
	CLOUD_TEXTURE,
	NUM_TEXTURES
} textureEnum;

typedef enum
{
	EARTH_VERTS,
	EARTH_NORMALS,
	EARTH_TEXTURE_COORDS,
	CLOUD_VERTS,
	CLOUD_NORMALS,
	CLOUD_TEXTURE_COORDS,
	NUM_VBO_ITEMS
} vboEnum;

typedef enum
{
	EARTH_SPHERE,
	CLOUD_SPHERE,
	NUM_VAO_ITEMS
} vaoEnum;

GLuint vao[NUM_VAO_ITEMS];
GLuint vbo[NUM_VBO_ITEMS];
GLuint texName[NUM_TEXTURES];
int globeVertCount;
int cloudSphereVertCount;
bool drawClouds = true;

//Let's have some mouse dragging rotation
int right_button_down = FALSE;
int left_button_down = FALSE;

int prevMouseX;
int prevMouseY;

double view_rotx = 0.0;
double view_roty = 0.0;
double view_rotz = 0.0;
double z_distance;

float earthRotation = 0.0;
float cloudRotation = 0.0;

//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;

//material properties
GLuint vPosition; //
GLuint vAmbientDiffuseColor; //Ambient and Diffuse can be the same for the material
GLuint vSpecularColor;
GLuint vSpecularExponent;
GLuint vNormal; //this is the standard name
GLuint texCoord;
GLuint dayTexMap;
GLuint nightTexMap;
GLuint specularTexMap;
GLuint cloudTexMap;


//light properties
GLuint light_position;
GLuint light_color;
GLuint ambient_light;

vec4* sphere_verts;
vec3* sphere_normals;
vec2* sphere_tex_coords;


//Modified slightly from the OpenIL tutorials (Copied from in class exercise)
ILuint loadTexFile(const char* filename)
{	
	/* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
    it can have different value (because it's just typedef of unsigned char), but this sould be
    avoided. Variable success will be used to determine if some function returned success or failure. */
	ILboolean success; 

	/* Before calling ilInit() version should be checked. */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		/* wrong DevIL version */
		printf("Wrong IL version");
		exit(1);
	}
 
	success = ilLoadImage(filename); /* Loading of image from file */
	if (success)
	{ /* If no error occured: */

		//We need to figure out whether we have an alpha channel or not
		if(ilGetInteger(IL_IMAGE_BPP) == 3)
		{
			/* Convert every color component into unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); 
		}
		else if(ilGetInteger(IL_IMAGE_BPP) == 4)
		{
			success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		}
		else
		{
			success = false;
		}
		if (!success)
		{
			/* Error occured */
			printf("failed conversion to unsigned byte");
			exit(1);
		}
	}
	else
	{
		/* Error occured */
		printf("Failed to load image ");
		printf(filename);
		exit(1);
	}
}

void reshape(int width, int height)
{
	ww= width;
	wh = height;
	//field of view angle, aspect ratio, closest distance from camera to object, largest distanec from camera to object
	p = Perspective(45.0, (float)width/(float)height, 1.0, 100.0);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glViewport( 0, 0, width, height );
}


//In this particular case, our normal vectors and vertex vectors are identical since the sphere is centered at the origin
//For most objects this won't be the case, so I'm treating them as separate values for that reason
//This could also be done as separate triangle strips, but I've chosen to make them just triangles so I don't have to execute multiple glDrawArrays() commands
int generateSphere(float radius, int subdiv)
{
	float step = (360.0/subdiv)*(M_PI/180.0);

	int totalverts = ceil(subdiv/2.0)*subdiv * 6;

	if(sphere_normals)
	{
		delete[] sphere_normals;
	}
	sphere_normals = new vec3[totalverts];
	
	if(sphere_verts)
	{
		delete[] sphere_verts;
	}
	sphere_verts = new vec4[totalverts];

	if (sphere_tex_coords)
	{
		delete[] sphere_tex_coords;
	}
	sphere_tex_coords = new vec2[totalverts];

	int k = 0;	

	float xLength = 1.0/((2*M_PI)/step);
	float yLength = 1.0/((M_PI)/step);
	int iint = 0;
	int jint = 0;
	for(float i = -M_PI/2; i<=M_PI/2; i+=step)
	{		
		jint = 0;
		for(float j = -M_PI; j<=M_PI; j+=step)
		{			
			//triangle 1
			sphere_normals[k] = vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			sphere_verts[k] =   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			sphere_tex_coords[k] = vec2((jint)*xLength, (iint)*yLength);
			k++;
	
			sphere_normals[k] = vec3(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step));
			sphere_verts[k] =   vec4(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step), 1.0);
			sphere_tex_coords[k] = vec2((jint) * xLength, (iint+1)*yLength);
			k++;
			
			sphere_normals[k] = vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			sphere_verts[k] =   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			sphere_tex_coords[k] = vec2((jint+1) * xLength, (iint+1)*yLength);
			k++;

			//triangle 2
			sphere_normals[k] = vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			sphere_verts[k] =   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			sphere_tex_coords[k] = vec2((jint+1) * xLength, (iint+1)*yLength);
			k++;

			sphere_normals[k] = vec3(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i));
			sphere_verts[k] =   vec4(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i), 1.0);
			sphere_tex_coords[k] = vec2((jint+1) * xLength, (iint)*yLength);
			k++;

			sphere_normals[k] = vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			sphere_verts[k] =   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			sphere_tex_coords[k] = vec2(jint*xLength, iint*yLength);
			k++;

			jint++;
		}
		iint++;		
	}

	return totalverts;
}

//Connect all of the shader variables to local pointers
void setupShader(GLuint prog){
	glUseProgram( prog );
	model_view = glGetUniformLocation(prog, "model_view");
	projection = glGetUniformLocation(prog, "projection");
	
	// Setup Lights
	light_position = glGetUniformLocation(prog, "light_position");
	light_color = glGetUniformLocation(prog, "light_color");
	ambient_light = glGetUniformLocation(prog, "ambient_light");

	//Setup Earth
	glBindVertexArray( vao[EARTH_SPHERE] );

	glBindBuffer( GL_ARRAY_BUFFER, vbo[EARTH_VERTS] );
	vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[EARTH_NORMALS] );
	vNormal = glGetAttribLocation(prog, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	dayTexMap = glGetUniformLocation(prog, "dayTexture");
	glUniform1i(dayTexMap, DAY_TEXTURE);

	nightTexMap = glGetUniformLocation(prog, "nightTexture");
	glUniform1i(nightTexMap, NIGHT_TEXTURE);

	specularTexMap = glGetUniformLocation(prog, "specularTexture");
	glUniform1i(specularTexMap, SPECULAR_TEXTURE);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[EARTH_TEXTURE_COORDS] );
	texCoord = glGetAttribLocation(prog, "texCoord");
	glEnableVertexAttribArray(texCoord);
	glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Setup Clouds
	glBindVertexArray( vao[CLOUD_SPHERE] );

	glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_VERTS] );
	vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_NORMALS] );
	vNormal = glGetAttribLocation(prog, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	cloudTexMap = glGetUniformLocation(prog, "cloudTexture");
	glUniform1i(cloudTexMap, CLOUD_TEXTURE);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_TEXTURE_COORDS] );
	texCoord = glGetAttribLocation(prog, "texCoord");
	glEnableVertexAttribArray(texCoord);
	glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Don't forget to send the projection matrix to our new shader!
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
}

void display(void)
{
	// Set shader to what the key events have setup
	setupShader(activeProgram);

	// Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
	// Take care of any mouse rotations or panning
    mv = LookAt(vec4(0, 0, 10+z_distance, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));
	mv = mv * RotateX(115);
	mv = mv * RotateX(view_rotx) * RotateY(view_roty) * RotateZ(view_rotz);
	mv = mv * RotateZ(-45);
	
	// Setup lighting vectors
	vec4 lightVec = mv*vec4(-5000, 0, 0, 1);
	vec4 ambientLightVec = vec4(0.1, 0.1, 0.1, 1);
	vec4 lightColorVec = vec4(0.6, 0.6, 0.6, 1);

	//Send in lighting vectors
	glUniform4fv(ambient_light, 1, ambientLightVec);
	glUniform4fv(light_color, 1, lightColorVec);	
	glUniform4fv(light_position, 1, lightVec);		// Light will follow the object (Because of mv)
	mat4 preRotate = mv;							// Save this off for use with the clouds
	mv = mv * RotateZ(-earthRotation);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	// Draw the earth
	glBindVertexArray( vao[EARTH_SPHERE] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texName[DAY_TEXTURE]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texName[NIGHT_TEXTURE]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texName[SPECULAR_TEXTURE]);
	glDrawArrays( GL_TRIANGLES, 0, globeVertCount );

	// Draw the clouds if necessary
	if (drawClouds == true)
	{
		// Switch to cloud shader
		setupShader(cloudMapProgram);

		// Adjust for light position
		mv = preRotate * RotateZ(-cloudRotation);

		// Send in the lighting vectors
		glUniform4fv(ambient_light, 1, ambientLightVec);
		glUniform4fv(light_color, 1, lightColorVec);
		glUniform4fv(light_position, 1, lightVec);		// Light will follow the object (Because of mv)
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

		// Draw the clouds
		glBindVertexArray( vao[CLOUD_SPHERE] );
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texName[CLOUD_TEXTURE]);

		glDrawArrays( GL_TRIANGLES, 0, cloudSphereVertCount );
	}

    glFlush();
	
	/*start processing buffered OpenGL routines*/
	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y) {
	/*exit when the escape key is pressed*/
	if (key == 27)
		exit(0);

	// Original (Default view)
	if (key == 'd'){
		activeProgram = colorMapProgram;
	}
	
	// Specular Shader
	if (key == 's'){
		activeProgram = specularMapProgram;
	}  

	// All Features
	if (key == 'a')
	{
		activeProgram = allFeaturesMapProgram;
		drawClouds = true;
	}

	// Toggle the clouds
	if (key == 'c')
	{
		drawClouds = !drawClouds;
	}	

	glutPostRedisplay();
}

//Mouse drags = rotation
void mouse_dragged(int x, int y) {
	double thetaY, thetaX;
	if (left_button_down) {
		thetaY = 360.0 *(x-prevMouseX)/ww;    
		thetaX = 360.0 *(prevMouseY - y)/wh;
		prevMouseX = x;
		prevMouseY = y;
		view_rotx += thetaX;
		view_roty += thetaY;
	}
	else if (right_button_down) {
		z_distance = 5.0*(prevMouseY-y)/wh;
	}
  glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
  //establish point of reference for dragging mouse in window
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		left_button_down = TRUE;
		prevMouseX= x;
		prevMouseY = y;
    }
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		right_button_down = TRUE;
		prevMouseX = x;
		prevMouseY = y;
    }
    else if (state == GLUT_UP) 
	{
		left_button_down = FALSE;
		right_button_down = FALSE;
	}
}

void init() {

	// Clear to Black
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Create the earth
	globeVertCount = generateSphere(4, 40);

	// Create a vertex array object
    glGenVertexArrays( NUM_VAO_ITEMS, &vao[0] );

    // Create and initialize any buffer objects
	glBindVertexArray( vao[EARTH_VERTS] );
	glGenBuffers( NUM_VBO_ITEMS, &vbo[0] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[EARTH_VERTS] );
    glBufferData( GL_ARRAY_BUFFER, globeVertCount*sizeof(vec4), sphere_verts, GL_STATIC_DRAW);	

	// Load the Noramls
	glBindBuffer( GL_ARRAY_BUFFER, vbo[EARTH_NORMALS] );
	glBufferData( GL_ARRAY_BUFFER, globeVertCount*sizeof(vec3), sphere_normals, GL_STATIC_DRAW );

	// Load the texture points
	glBindBuffer( GL_ARRAY_BUFFER, vbo[EARTH_TEXTURE_COORDS] );
    glBufferData( GL_ARRAY_BUFFER, globeVertCount*sizeof(vec2), sphere_tex_coords, GL_STATIC_DRAW);

	// Create the cloud
	cloudSphereVertCount = generateSphere(4.1, 50);

    // Create and initialize any buffer objects
	glBindVertexArray( vao[CLOUD_SPHERE] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_VERTS] );
    glBufferData( GL_ARRAY_BUFFER, cloudSphereVertCount*sizeof(vec4), sphere_verts, GL_STATIC_DRAW);	

	// Load the Normals
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_NORMALS] );
	glBufferData( GL_ARRAY_BUFFER, cloudSphereVertCount*sizeof(vec3), sphere_normals, GL_STATIC_DRAW );

	// Load the texture points
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CLOUD_TEXTURE_COORDS] );
    glBufferData( GL_ARRAY_BUFFER, cloudSphereVertCount*sizeof(vec2), sphere_tex_coords, GL_STATIC_DRAW);

	// Load shaders and use the resulting shader program
    colorMapProgram = InitShader( "vshader-color.glsl", "fshader-color.glsl" );
	specularMapProgram = InitShader( "vshader-specular.glsl", "fshader-specular.glsl" );
	allFeaturesMapProgram = InitShader( "vshader-allfeatures.glsl", "fshader-allfeatures.glsl" );
	cloudMapProgram = InitShader( "vshader-clouds.glsl", "fshader-clouds.glsl" );
	activeProgram = colorMapProgram;
	setupShader(activeProgram);

	// Setup and Load the Textures
	ILuint ilTexID[NUM_TEXTURES]; /* ILuint is a 32bit unsigned integer.
    //Variable texid will be used to store image name. */

	ilInit(); /* Initialization of OpenIL */
	ilGenImages(NUM_TEXTURES, ilTexID); /* Generation of three image names for OpenIL image loading */
	glGenTextures(NUM_TEXTURES, texName); //and we eventually want the data in an OpenGL texture

	// Load Day Texture
	ilBindImage(ilTexID[DAY_TEXTURE]);
	loadTexFile("images/Earth.png");
	glBindTexture(GL_TEXTURE_2D, texName[DAY_TEXTURE]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);	
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	// Load Night Texture
	ilBindImage(ilTexID[NIGHT_TEXTURE]);
	loadTexFile("images/EarthNight.png");
	glBindTexture(GL_TEXTURE_2D, texName[NIGHT_TEXTURE]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);	
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	// Load Specular Texture
	ilBindImage(ilTexID[SPECULAR_TEXTURE]);
	loadTexFile("images/EarthSpec.png");
	glBindTexture(GL_TEXTURE_2D, texName[SPECULAR_TEXTURE]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);	
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	// Load Cloud Texture
	ilBindImage(ilTexID[CLOUD_TEXTURE]);
	loadTexFile("images/earthcloudmap.png");
	glBindTexture(GL_TEXTURE_2D, texName[CLOUD_TEXTURE]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);	
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
	   ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);

    ilDeleteImages(NUM_TEXTURES, ilTexID); //we're done with OpenIL, so free up the memory	

	//Only draw the things in the front layer
	glEnable(GL_DEPTH_TEST);

	//Had issues with the clouds, so enable front face culling
	glEnable(GL_CULL_FACE);

	// Enable blending for when clouds are drawn
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void my_timer(int v) 
{	
	// Rotate the earth and clouds
	earthRotation += 0.05;
	cloudRotation += 0.04;

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
	glutCreateWindow("Assignment 5 - Ross Anderson");  

	glewExperimental = GL_TRUE;

	glewInit();
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(reshape);
	glutTimerFunc(500, my_timer, 60);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_dragged);

	glutMainLoop();
	return 0;
}