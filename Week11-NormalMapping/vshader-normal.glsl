#version 150

in vec4 vPosition;
in vec2 texCoord;
in vec4 vNormal;
in vec4 vTangent;
in vec4 vAmbientDiffuseColor;
in vec4 vSpecularColor;
in float vSpecularExponent;

out vec2 ftexCoord;
out vec3 L;
out vec3 H;
out vec4 AmbientDiffuseColor;
out vec4 SpecularColor;
out float SpecularExponent;

uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 light_position;


void
main()
{	
	vec3 tN = normalize(model_view * vNormal).xyz;
	vec3 tT = normalize(model_view * vTangent).xyz;
	vec3 tB = cross(tN, tT);
	//if tN and tT are not perpendicular, we would have to correct that at this point

	vec4 veyepos = model_view*vPosition;

	//construct a change in coordinate frame matrix
	mat4 local = mat4(vec4(tB,0), vec4(tT,0), vec4(tN, 0), veyepos);

	// These are the same as phong, just multiplying local by some
	L = (local * vec4(normalize( light_position.xyz - veyepos.xyz), 0)).xyz;
	vec3 E = (local * vec4(normalize(-veyepos.xyz), 0)).xyz;
	H = normalize(L+E);

	ftexCoord = texCoord;

	gl_Position = projection * model_view*vPosition;

	AmbientDiffuseColor = vAmbientDiffuseColor;
	SpecularColor = vSpecularColor;
	SpecularExponent = vSpecularExponent;
}