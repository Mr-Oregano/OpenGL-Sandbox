#version 450 core

layout(location = 0) in vec4 a_Position;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
	
	gl_Position = a_Position;
	
}