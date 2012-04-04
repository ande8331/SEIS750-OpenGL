#version 150

in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal; // must be in position 3 for teapot
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 ambient_light;
out vec4 color;

in vec4 vAmbient;
in vec4 vDiffuse;
in vec4 vSpecular;
in float vSpecularExponent;
out vec4 fvAmbient;
out vec4 fvDiffuse;
out vec4 fvSpecular;
out float fvSpecularExponent;
out vec4 fvAmbientLight;
out vec4 position;
out vec3 vN;
out vec4 veyepos;

void
main()
{
	veyepos = model_view*vPosition;
	vec4 normal = vec4(vNormal, 0);		// Converts the vec3 to a vec4
	vec3 N = normalize(( model_view * normal).xyz);  // apply any mv transformations to the normal (needed to be vec4 to allow 4x4 to multiply it)

    gl_Position = projection*model_view*vPosition;
	position = veyepos;

	color = vColor;

	// Ambient
	fvAmbientLight = ambient_light;

	vN = N;
	fvAmbient = vAmbient;
	fvDiffuse = vDiffuse;
	fvSpecular = vSpecular;
	fvSpecularExponent = vSpecularExponent;
}