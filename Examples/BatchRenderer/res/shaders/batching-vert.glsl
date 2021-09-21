#version 450 core

layout (location = 0) in vec4 a_Color;
layout (location = 1) in vec4 a_PositionSizeData;
layout (location = 2) in float a_Rotation;
layout (location = 3) in float a_TextureSelection;
layout (location = 4) in vec2 a_TexelSize;

out float v_Rotation;
out vec2 v_TexelSize;

out vec4 v_Color;
flat out int v_TextureSelection;

void main()
{

	gl_Position = a_PositionSizeData; 
	v_Rotation = a_Rotation;

	v_Color = a_Color;
	v_TextureSelection = int(a_TextureSelection);

	v_TexelSize = a_TexelSize;

}