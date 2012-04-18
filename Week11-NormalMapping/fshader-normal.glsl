#version 150

in vec2 ftexCoord;
in vec3 L;
in vec3 H;
in vec4 AmbientDiffuseColor;
in vec4 SpecularColor;
in float SpecularExponent;

uniform vec4 light_color;
uniform vec4 ambient_light;
uniform sampler2D texture;
uniform sampler2D normalMap;

out vec4  fColor;

void main()
{

	vec3 N = (texture2D(normalMap, ftexCoord)*2 - 1).xyz;
	vec4 amb = AmbientDiffuseColor * ambient_light;
	vec4 diff = max(dot(L,N), 0.0) * AmbientDiffuseColor * light_color;
	vec4 spec = pow( max (dot(N,H), 0.0), SpecularExponent) *  SpecularColor * light_color  ;
	
	if(dot(L,N) < 0.0)
	{
		spec = vec4(0,0,0,1);
	}
	fColor = amb + diff + spec;
	
	fColor *= texture2D(texture, ftexCoord);	
}