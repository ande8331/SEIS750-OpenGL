#version 150
uniform vec4 ambient_light;
in vec4 fvAmbientDiffuseColor;
uniform vec4 light_color;
in float fvSpecularExponent;
in vec4 fvSpecularColor;
in vec3 N;
in vec3 V;
in vec3 H;
in vec3 L;
out vec4  fColor;

void main()
{    
	vec4 ambient = ambient_light*fvAmbientDiffuseColor;
	vec4 diffuse = light_color * fvAmbientDiffuseColor * max(0.0, dot(L, N));
	vec4 specular = light_color * fvSpecularColor * pow(max(0.0, dot(N, H)), fvSpecularExponent); // Specular
	
	if (dot (L, N) < 0)
	{
		specular = vec4(0, 0, 0, 1); // Certain positionings cause problems (being on wrong side of object), test for it, throw it out
	}

	fColor = ambient + diffuse + specular;
}