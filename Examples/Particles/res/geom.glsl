#version 450 core

#define SQRT2_2 0.707106781187
#define ANGLE_OFFSET 1.57079632679

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 u_ViewProjection;

in vec4 v2g_Color[];
in float v2g_Rotation[];
in float v2g_Size[];
in float v2g_Life[];

out vec4 g2f_Color;
out float g2f_Life;

void CreateVertex(vec2 pos, vec2 orientation)
{
    float rot_xcomp = orientation.x * cos(orientation.y);
    float rot_ycomp = orientation.x * sin(orientation.y);

    vec2 newPos = pos + vec2(rot_xcomp, rot_ycomp);
    gl_Position = u_ViewProjection * vec4(newPos, 0.0, 1.0);
    EmitVertex();
}

void main() 
{
    vec2 pos = gl_in[0].gl_Position.xy;
    float size = v2g_Size[0];

    float radius = size * SQRT2_2;
    float theta = radians(v2g_Rotation[0]) + ANGLE_OFFSET / 2.0;

    g2f_Color = v2g_Color[0];
    g2f_Life = v2g_Life[0];

    CreateVertex(pos, vec2(radius, theta));
    CreateVertex(pos, vec2(radius, theta + ANGLE_OFFSET));
    CreateVertex(pos, vec2(radius, theta - ANGLE_OFFSET));
    CreateVertex(pos, vec2(radius, theta + 2.0 * ANGLE_OFFSET));

    EndPrimitive();
}