#version 150

in vec4 vPosition;
in vec4 vColor;
uniform mat4 model_view;
uniform mat4 projection;
out vec4 color;

in vec2 texCoord;
out vec2 fTexCoord;

in vec3 vNormal; // must be in position 3 for teapot
uniform vec4 light_position;
in vec4 vAmbientDiffuseColor;
in float vSpecularExponent;
in vec4 vSpecularColor;

out vec4 fvAmbientDiffuseColor;
out float fvSpecularExponent;
out vec4 fvSpecularColor;
out vec4 position;
out vec3 vN;

void main()
{
	vec4 veyepos = model_view*vPosition;
	vec4 normal = vec4(vNormal, 0);		// Converts the vec3 to a vec4
	vec3 L = normalize( light_position.xyz - veyepos.xyz ); // use vec3 because it works better in a dot product

	vec3 N = normalize(( model_view * normal).xyz);  // apply any mv transformations to the normal (needed to be vec4 to allow 4x4 to multiply it
	vec3 V = normalize(-veyepos.xyz);		// View vector
	vec3 H = normalize(L+V);	// Average (Normalize makes this an effective average)
	gl_Position = projection*veyepos;
	position = veyepos;
	fvAmbientDiffuseColor = vAmbientDiffuseColor;
	fvSpecularExponent = vSpecularExponent;
	fvSpecularColor = vSpecularColor;
	vN = N;

	fTexCoord = texCoord;
}