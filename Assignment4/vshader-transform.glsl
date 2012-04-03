#version 150

in vec4 vPosition;
in vec4 vAmbientDiffuseColor;
in vec3 vNormal; // must be in position 3 for teapot
in vec4 vColor;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 ambient_light;
out vec4 color;

uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 light_direction;
//uniform vec4 light_cutoffangle;

in vec4 vAmbient;
in vec4 vDiffuse;
in vec4 vSpecular;
out vec4 fvAmbient;
out vec4 fvDiffuse;
out vec4 fvSpecular;


out vec4 fvAmbientLight;

out vec4 fvLightColor;
//out vec4 fvLightDirection;
//out vec4 fvLightCutoffangle;
out vec4 position;
out vec3 vN;
out vec4 fvPosition;
out vec3 vL;
out vec3 vLD;

void
main()
{
	vec4 veyepos = model_view*vPosition;
	vec4 normal = vec4(vNormal, 0);		// Converts the vec3 to a vec4
	vec3 L = normalize( light_position.xyz - veyepos.xyz ); // use vec3 because it works better in a dot product
	
	vec3 N = normalize(( model_view * normal).xyz);  // apply any mv transformations to the normal (needed to be vec4 to allow 4x4 to multiply it
	
	vec3 LD = normalize(light_direction.xyz);

    gl_Position = projection*model_view*vPosition;
	position = veyepos;

	color = vColor;

	// Ambient
	fvAmbientLight = ambient_light;

	// Spot
	fvLightColor=light_color;
	//fvLightCutoffangle =light_cutoffangle;
	fvPosition = vPosition;

	vN = N;
	vL = L;
	vLD = LD;
	fvAmbient = vAmbient;
	fvDiffuse = vDiffuse;
	fvSpecular = vSpecular;
}