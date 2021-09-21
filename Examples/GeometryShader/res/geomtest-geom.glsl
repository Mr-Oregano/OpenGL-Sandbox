#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float u_Side;
uniform mat4 u_ViewProjection;

void main() {    

    gl_Position = u_ViewProjection * (gl_in[0].gl_Position + vec4(-u_Side, -u_Side, 0.0, 0.0)); 
    EmitVertex();

    gl_Position = u_ViewProjection * (gl_in[0].gl_Position + vec4(u_Side, -u_Side, 0.0, 0.0)); 
    EmitVertex();

    gl_Position = u_ViewProjection * (gl_in[0].gl_Position + vec4(-u_Side, u_Side, 0.0, 0.0)); 
    EmitVertex();

    gl_Position = u_ViewProjection * (gl_in[0].gl_Position + vec4(u_Side, u_Side, 0.0, 0.0)); 
    EmitVertex();

    EndPrimitive();

}  