#version 450 core

layout (location = 0) out vec4 o_Color;

// NOTE: This uniform buffer will be associated with the UBO
//		 at binding point 0. Useful if you know the binding points
//		 ahead of time.
//
layout (std140, binding = 0) uniform Uniforms
{
	float x;
	vec4 color;
};

void main()
{
	o_Color = color;
}