#version 450 core

layout (location = 0) out vec4 o_Color;

layout (std140, binding = 0) uniform Uniforms
{
	vec4 color;
};

void main()
{
	o_Color = color;
}