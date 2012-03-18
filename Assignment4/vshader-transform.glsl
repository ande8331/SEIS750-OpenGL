#version 150

in vec4 vPosition;
in vec4 vAmbientDiffuseColor;
in vec4 vColor;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 ambient_light;
out vec4 color;

void
main()
{
    gl_Position = projection*model_view*vPosition;

	vec4 ambient = ambient_light*vColor;
	
	color = ambient;
	//color = vColor;
}