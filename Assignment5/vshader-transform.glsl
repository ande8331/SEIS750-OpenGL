#version 150

in vec4 vPosition;
in vec4 vColor;
uniform mat4 model_view;
uniform mat4 projection;
out vec4 color;

in vec2 texCoord;
out vec2 fTexCoord;

void
main()
{
    gl_Position = projection*model_view*vPosition;
	color = vColor;

	fTexCoord = texCoord;
}