#version 450 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TexCoords;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec2 v2f_TexCoords;

void main()
{
	gl_Position = u_ViewProjection * u_Model * a_Position;
	v2f_TexCoords = a_TexCoords;
}