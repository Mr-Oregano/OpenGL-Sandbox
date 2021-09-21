#version 450 core

layout(location = 0) out vec4 a_Color;

uniform sampler2D u_Sampler2D;

in vec2 v2f_TexCoords;

void main()
{
	a_Color = texture(u_Sampler2D, v2f_TexCoords);
}