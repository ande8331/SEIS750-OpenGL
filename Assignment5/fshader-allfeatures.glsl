#version 150
in vec4 color;
out vec4  fColor;
in vec2 fTexCoord;
uniform sampler2D dayTexture;
uniform sampler2D nightTexture;
uniform sampler2D specularTexture;
uniform vec4 ambient_light;
in vec4 fvAmbientDiffuseColor;
uniform vec4 light_color;
uniform vec4 light_position;
in float fvSpecularExponent;
in vec4 fvSpecularColor;
in vec3 vN;
in vec4 position;


void main()
{
	vec3 L = normalize(light_position.xyz -position.xyz);
	vec3 E = normalize (-position.xyz);
	vec3 N = normalize (vN);
	vec3 H = normalize (L+E);

	vec4 tmpfvAmbientDiffuseColor = texture2D(dayTexture, fTexCoord);
	vec4 specMap = texture2D(specularTexture, fTexCoord);

	vec4 ambient = ambient_light*tmpfvAmbientDiffuseColor;
	vec4 diffuse = light_color * tmpfvAmbientDiffuseColor * max(0.0, dot(L, N));
	vec4 specular = light_color * specMap * pow(max(0.0, dot(N, H)), specMap.w*200); // Specular
	
	if (dot (L, N) < 0.0)
	{
		ambient = texture2D(nightTexture, fTexCoord);
		diffuse = vec4(0,0,0,1);
		specular = vec4(0, 0, 0, 1); // Certain positionings cause problems (being on wrong side of object), test for it, throw it out
	}
	else if (dot (L,N) < 0.20)
	{
		float dayAverage = (dot(L,N)/0.20);
		float nightAverage = 1 - dayAverage;
		ambient = (ambient*dayAverage) + (texture2D(nightTexture, fTexCoord) * nightAverage);
		diffuse = (diffuse*dayAverage);
		specular = (specular*dayAverage);
	}

	fColor = ambient + diffuse + specular;
	fColor.w = 1.0;	
}