#version 150
in vec4 color;
out vec4  fColor;
in vec3 vN;
in vec4 position;
in vec4 fvPosition;

uniform vec4 vAmbient, vDiffuse, vSpecular;

in vec4 fvAmbientLight;
in vec4 fvLightColor;
in vec4 fvLightCutoffangle;

in vec3 vL;
in vec3 vLD;

void main()
{
	vec3 L = normalize(vL);
	vec3 E = normalize (position.xyz);
	vec3 N = normalize (vN);
	vec3 H = normalize (L+E);
	vec3 fvLightDirection = normalize (vLD);


	//vec4 ambient = fvAmbientLight*color;
	vec4 ambient = vAmbient * fvAmbientLight;

	vec4 diffuse = vec4(0,0,0,0);

	//if(dot(L,  fvLightDirection.xyz) > cos(fvLightCutoffangle.x))
	{
		diffuse = fvLightColor * vDiffuse * max(0.0, dot(L, N));
	}
	
	vec4 specular = vec4(0,0,0,0);

	if (dot (L, N) < 0)
	{
		specular = vec4(0, 0, 0, 1); // Certain positionings cause problems (being on wrong side of object), test for it, throw it out
	}

	fColor = ambient + diffuse + specular;
}