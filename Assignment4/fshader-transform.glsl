#version 150
in vec4 color;
out vec4  fColor;
in vec3 vN;
in vec4 position;

in vec4 fvAmbient;
in vec4 fvDiffuse;
in vec4 fvSpecular;
in float fvSpecularExponent;
in vec4 fvAmbientLight;

uniform float light_cutoffangle[4];
uniform vec4 light_direction[4];
uniform vec4 light_position[4];
uniform vec4 light_color[4];

in vec4 veyepos;

void main()
{
	vec3 N = normalize (vN);
	vec3 E = normalize (-position.xyz);
	vec3 L[4];
	vec3 H[4];
	vec3 LD[4];

	for (int i = 0; i < 4; i++)
	{
		L[i] = normalize( light_position[i].xyz - veyepos.xyz ); // use vec3 because it works better in a dot product
		LD[i] = normalize( light_direction[i].xyz );
		H[i] = normalize (L[i]+E);
	}

	vec4 ambient = fvAmbient * fvAmbientLight;
	vec4 diffuse = vec4(0,0,0,1);
	vec4 specular = vec4(0,0,0,1);

	for (int i = 0; i < 4; i++)
	{
		if(dot(L[i], LD[i].xyz) > cos(light_cutoffangle[i]))
		{
			diffuse += light_color[i] * fvDiffuse * max(0.0, dot(L[i], N));

			if (dot (L[i], N) > 0)
			{
				// Certain positionings cause problems (being on wrong side of object), test for it, throw it out
				specular += light_color[i] * fvSpecular * pow(max(0.0, dot(N, H[i])), fvSpecularExponent); // Specular
				//specular += light_color[i] * fvSpecular * pow(1, fvSpecularExponent); // Specular
			}
		}
	}

	fColor = ambient + diffuse + specular;
	//fColor = vec4(N, 1);		// Debugging - Check the normals out
	fColor.a = 1.0;
}