#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBiTangent;
layout (location = 5) in vec3 inColor;

out VertexData {
    vec3 worldPosition;
    vec3 normal;
    vec3 color;
} o;

uniform mat4 MVP;
uniform mat4 model;

void main()
{
    gl_Position = MVP * vec4(inPosition, 1.0);

    o.worldPosition = vec3(model * vec4(inPosition, 1.0));
    o.normal = inNormal;
    o.color = inColor;
}