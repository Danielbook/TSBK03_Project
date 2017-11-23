#version 150

out vec4 out_Color;
uniform vec4 color;
in float shade;
in vec2 texCoord;

void main(void)
{
	out_Color = color * shade;
//	out_Color = color;
//	out_Color = vec4(1.0, 0.0, 0.0, 1.0);;
//	out_Color = vec4(texCoord.s, texCoord.t, 1.0, 1.0);;
}
