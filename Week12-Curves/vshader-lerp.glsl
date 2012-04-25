#version 150

uniform float t;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec2 controlPoints[4];

out vec4 color;

void
main()
{	
	color = vec4(0,0,1,1);
	//this will calculate the x and y coordinates as a vector, then add on the z and w coordinates
	vec4 position = vec4(controlPoints[1]*(1-t) + t*controlPoints[2], 0, 1);
	gl_Position = projection * model_view * position;
	

}