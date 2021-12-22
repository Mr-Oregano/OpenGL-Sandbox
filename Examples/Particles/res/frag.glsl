#version 450 core

layout (location = 0) out vec4 o_Color;

in vec4 g2f_Color;
in float g2f_Life;

void main()
{
	o_Color = g2f_Color;
}