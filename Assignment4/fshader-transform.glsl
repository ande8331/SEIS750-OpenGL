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
in vec4 fvLightPosition;
uniform float light_cutoffangle[4];
uniform vec4 light_direction[4];

in vec3 vL;
in vec3 vLD;
in vec4 veyepos;

void main()
{
	//vec3 L = normalize(vL);
	vec3 L = normalize( fvLightPosition.xyz - veyepos.xyz ); // use vec3 because it works better in a dot product
	vec3 E = normalize (position.xyz);
	vec3 N = normalize (vN);
	vec3 H = normalize (L+E);
//	vec3 fvLightDirection = normalize (vLD);
vec3 fvLightDirection = normalize(light_direction[0].xyz);
	vec4 ambient = fvAmbient * fvAmbientLight;
	//vec4 ambient = fvAmbientLight*color;
	//ambient = color;
	vec4 diffuse = vec4(0,0,0,1);
	vec4 specular = vec4(0,0,0,1);

	if(dot(L, fvLightDirection.xyz) > cos(light_cutoffangle[0]))
	{
		//diffuse = vec4(1.0, 1.0, 1.0, 1.0);
		diffuse = fvLightColor * fvDiffuse * max(0.0, dot(L, N));
		//diffuse = color * max(0.0, dot(L, N));
		//diffuse = color;
	}

	if (dot (L, N) < 0)
	{
		specular = vec4(0, 0, 0, 1); // Certain positionings cause problems (being on wrong side of object), test for it, throw it out
	}

	//fColor = ambient + diffuse + specular;
	fColor = vec4(N, 1);
	fColor.a = 1.0;
}