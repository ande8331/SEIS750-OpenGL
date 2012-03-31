#version 150
in vec4 color;
out vec4  fColor;
in vec3 vN;
in vec4 position;

in vec4 fvAmbientLight;

in vec4 fvLightPosition;
in vec4 fvLightColor;
in vec4 fvLightDirection;
in vec4 fvLightCutoffangle;

void main()
{
	vec3 L = normalize(fvLightPosition.xyz -position.xyz);
	vec3 E = normalize (-position.xyz);
	vec3 N = normalize (vN);
	vec3 H = normalize (L+E);

	vec4 ambient = (fvAmbientLight)*color;
	vec4 diffuse = vec4(0,1,0,0);
	vec4 specular = vec4(0,0,0,0);

	if (dot (L, N) < 0)
	{
		specular = vec4(0, 0, 0, 1); // Certain positionings cause problems (being on wrong side of object), test for it, throw it out
	}

	fColor = ambient + diffuse + specular;
}