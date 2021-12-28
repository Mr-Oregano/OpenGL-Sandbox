#version 450 core

layout (location = 0) in vec4 a_Color;
layout (location = 1) in vec4 a_Position;
layout (location = 2) in float a_Size;
layout (location = 3) in float a_Rotation;
layout (location = 4) in float a_Life;

out vec4 v2g_Color;
out float v2g_Rotation;
out float v2g_Size;
out float v2g_Life;

void main()
{
	gl_Position = a_Position;

	v2g_Color = a_Color;
	v2g_Rotation = a_Rotation;
	v2g_Size = a_Size;
	v2g_Life = a_Life;
}