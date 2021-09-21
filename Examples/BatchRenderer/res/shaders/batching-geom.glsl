#version 450 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

in vec4 v_Color[];
in float v_Rotation[];
in vec2 v_TexelSize[];
flat in int v_TextureSelection[];

out vec4 vf_Color;
out vec2 vf_TextureCoords;
out vec2 vf_TexelSize;
flat out int vf_TextureSelection;

void CreateVertex(vec2 pos, vec2 orientation, vec2 texCoords)
{

    float rot_xcomp = orientation.x * cos(orientation.y);
    float rot_ycomp = orientation.x * sin(orientation.y);

    vec2 newPos = pos + vec2(rot_xcomp, rot_ycomp);
    gl_Position = u_ViewProjection * u_Model * vec4(newPos, 0.0, 1.0);
    vf_TextureCoords = texCoords;
    EmitVertex();

}
void main() {

    vec2 pos = gl_in[0].gl_Position.xy;
    vec2 size = gl_in[0].gl_Position.zw;

    float radius = sqrt(size.x * size.x + size.y * size.y) / 2.0;
    float angleChangeX = 2.0 * asin(size.x / (2.0 * radius));
    float angleChangeY = 2.0 * asin(size.y / (2.0 * radius));
    float theta = radians(v_Rotation[0]) + angleChangeY / 2.0;

    vf_TexelSize = v_TexelSize[0];
    vf_Color = v_Color[0];
    vf_TextureSelection = v_TextureSelection[0];

    CreateVertex(pos, vec2(radius, theta), vec2(1.0, 1.0));
    CreateVertex(pos, vec2(radius, theta + angleChangeX), vec2(0.0, 1.0));
    CreateVertex(pos, vec2(radius, theta - angleChangeY), vec2(1.0, 0.0));
    CreateVertex(pos, vec2(radius, theta + angleChangeY + angleChangeX), vec2(0.0, 0.0));

    EndPrimitive();

}