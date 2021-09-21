#version 450 core

layout (location = 0) out vec4 a_Color;

uniform vec2 u_Mouse;
uniform vec2 u_Resolution;
uniform float u_Time;

uniform vec2 u_Origin = vec2(0.5, 0.5);
uniform vec2 u_Destination; // u_Mouse

float drawLine(vec2 coordinate, vec2 origin, vec2 dest, float thickness)
{
	float theta = (coordinate.x - 0.5) * 30.0;
	float offset = sin(theta) * 0.1 + 0.5;
	float dOdt = cos(theta) * 0.1;
	float height = clamp(abs(dOdt), thickness, 1.0) * thickness;
	float lower = offset + height / 2.0;
	float higher = offset - height / 2.0;
	float smoothness = height;

	float x = step(0.5, coordinate.x);
	float y = smoothstep(higher - smoothness / 2.0, higher + smoothness / 2.0, coordinate.y) - 
			  smoothstep(lower - smoothness / 2.0, lower + smoothness / 2.0, coordinate.y);

	return x * y;
}
void main()
{
	vec2 uv = gl_FragCoord.xy / u_Resolution;
	uv.y = 1.0 - uv.y;

	a_Color = vec4(drawLine(uv, u_Origin, u_Mouse, 0.5));
}