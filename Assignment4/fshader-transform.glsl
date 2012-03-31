#version 150
in vec4 color;
out vec4  fColor;

in vec4 fvAmbientLight;

in vec4 fvLightPosition;
in vec4 fvLightColor;
in vec4 fvLightDirection;
in vec4 fvLightCutoffangle;

void main()
{
	vec4 ambient = (fvAmbientLight)*color;
	
	fColor = ambient;
}