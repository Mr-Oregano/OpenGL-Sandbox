#version 450 core

layout (location = 0) out vec4 o_Color;

in vec4 g2f_Color;
in float g2f_Life;
in vec2 g2f_TexCoords;

uniform sampler2D u_Texture;

void main()
{
	vec4 texSample = texture(u_Texture, g2f_TexCoords);
	o_Color = texSample * g2f_Color;
}