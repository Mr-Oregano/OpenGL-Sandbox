#version 450 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 u_ViewProjection;

in vec4 v2g_Color[];
in float v2g_Rotation[];
in float v2g_Size[];
in float v2g_Life[];

out vec4 g2f_Color;
out float g2f_Life;
out vec2 g2f_TexCoords;

void main() 
{
    vec2 pos = gl_in[0].gl_Position.xy;
    float size = v2g_Size[0];

    vec2 v00 = vec2(-size / 2.0);
    vec2 v01 = vec2(-size / 2.0, size / 2.0);
    vec2 v11 = vec2(size / 2.0);
    vec2 v10 = vec2(size / 2.0, -size / 2.0);

    float rotation = radians(v2g_Rotation[0]);
    
    mat2 model = mat2(
        vec2(cos(rotation), sin(rotation)), 
        vec2(-sin(rotation), cos(rotation))
    );

    g2f_Color = v2g_Color[0];
    g2f_Life = v2g_Life[0];

    vec2 newPos = pos + model * v11;
    gl_Position = u_ViewProjection * vec4(newPos, 0.0, 1.0);
    g2f_TexCoords = vec2(1.0);
    EmitVertex();

    newPos = pos + model * v01;
    gl_Position = u_ViewProjection * vec4(newPos, 0.0, 1.0);
    g2f_TexCoords = vec2(0.0, 1.0);
    EmitVertex();

    newPos = pos + model * v10;
    gl_Position = u_ViewProjection * vec4(newPos, 0.0, 1.0);
    g2f_TexCoords = vec2(1.0, 0.0);
    EmitVertex();

    newPos = pos + model * v00;
    gl_Position = u_ViewProjection * vec4(newPos, 0.0, 1.0);
    g2f_TexCoords = vec2(0.0);
    EmitVertex();

    EndPrimitive();
}