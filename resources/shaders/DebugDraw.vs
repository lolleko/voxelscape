#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

uniform mat4 VP;

out VertexData {
    vec3 color;
} o;

void main()
{
    gl_Position = VP * vec4(inPosition, 1.f);
    o.color = inColor;
}