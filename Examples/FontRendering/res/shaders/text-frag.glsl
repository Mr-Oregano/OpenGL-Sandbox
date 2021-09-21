#version 450 core

layout(location = 0) out vec4 a_Color;

in vec2 v_TexCoords;

uniform sampler2D u_Atlas;
uniform float u_AtlasTexels;

uniform float u_Edge = 0.1;
uniform float u_Width = 0.5;

uniform float u_BorderEdge = 0.1;
uniform float u_BorderWidth = 0.9;

uniform vec4 u_TextColor;

void main()
{
	float realEdge = u_Edge * u_AtlasTexels * abs(dFdx(v_TexCoords.x));
	float realBorderEdge = u_BorderEdge * u_AtlasTexels * abs(dFdx(v_TexCoords.x));
	float realWidth = (u_Width + u_Edge) - realEdge;

	float dist = 1.0 - texture(u_Atlas, v_TexCoords).a;
	float alpha = 1.0 - smoothstep(u_Width, u_Width + realEdge, dist);
	float outline = 1.0 - smoothstep(u_BorderWidth, u_BorderWidth + realBorderEdge, dist);

	float finalAlpha = alpha + (1.0 - alpha) * outline;
	vec3 finalColor = mix(vec3(0.0, 0.0, 0.0), u_TextColor.rgb, alpha / finalAlpha);

	a_Color = vec4(finalColor, finalAlpha);
}