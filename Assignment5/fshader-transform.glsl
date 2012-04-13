#version 150
in vec4 color;
out vec4  fColor;
in vec2 fTexCoord;
uniform sampler2D texture;
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

	vec4 ambient = ambient_light*fvAmbientDiffuseColor;
	vec4 diffuse = light_color * fvAmbientDiffuseColor * max(0.0, dot(L, N));

	fColor = (ambient*texture2D(texture, fTexCoord)) + (diffuse * texture2D(texture, fTexCoord));
}