#version 150

in vec2 vPosition;
in vec4 vColor;

out vec4 color;

uniform mat4 model_view;
uniform mat4 projection;



void
main()
{	
	color = vColor;
	gl_Position = projection * model_view*vec4(vPosition, 0, 1);
	

}