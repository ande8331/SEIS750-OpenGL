#version 150
in vec4 color;
out vec4  fColor;
in vec3 vN;
in vec4 position;
in vec4 fvPosition;

in vec4 fvAmbient;
in vec4 fvDiffuse;
in vec4 fvSpecular;

in vec4 fvAmbientLight;
in vec4 fvLightColor;
uniform float light_cutoffangle;

in vec3 vL;
in vec3 vLD;

void main()
{
	vec3 L = normalize(vL);
	vec3 E = normalize (position.xyz);
	vec3 N = normalize (vN);
	vec3 H = normalize (L+E);
	vec3 fvLightDirection = normalize (vLD);

	// This is the line it should be, but it goes completely black right now	
	vec4 ambient = fvAmbient * fvAmbientLight;
	//vec4 ambient = fvAmbientLight*color;
	vec4 diffuse = vec4(0,0,0,1);
	vec4 specular = vec4(0,0,0,0);
	
	// Debug Junk
	//fvLightDirection = normalize(vec3(0, 1, 0));
	//L = normalize(vec3(0, 5, 0));

	// Get a light when in degrees, have to flip the greater than to less
	// than for radians though
	float fvLightCutoffangle = light_cutoffangle;
	if(dot(L, fvLightDirection.xyz) > cos(fvLightCutoffangle))
	//if(dot(L, fvLightDirection.xyz) > 0.94)
	{
		//diffuse = vec4(1.0, 1.0, 1.0, 1.0);
		diffuse = fvLightColor * fvDiffuse * max(0.0, dot(L, N));
		//diffuse = color * max(0.0, dot(L, N));
	}

	if (dot (L, N) < 0)
	{
		specular = vec4(0, 0, 0, 1); // Certain positionings cause problems (being on wrong side of object), test for it, throw it out
	}

	fColor = ambient + diffuse + specular;
	fColor.a = 1.0;
}