#version 150
in vec4 color;
out vec4  fColor;
in vec2 fTexCoord;
uniform sampler2D texture;
void main()
{
    //fColor = color;
	fColor = texture2D(texture, fTexCoord);
}