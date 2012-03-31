#version 150

in vec4 vPosition;
in vec4 vAmbientDiffuseColor;
in vec4 vColor;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 ambient_light;
out vec4 color;

uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 light_direction;
uniform vec4 light_cutoffangle;

out vec4 fvAmbientLight;

out vec4 fvLightPosition;
out vec4 fvLightColor;
out vec4 fvLightDirection;
out vec4 fvLightCutoffangle;


void
main()
{
    gl_Position = projection*model_view*vPosition;

	color = vColor;

	fvAmbientLight = ambient_light;

	fvLightPosition=light_position;
	fvLightColor=light_color;
	fvLightDirection=light_direction;
	fvLightCutoffangle =light_cutoffangle;
}