#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBiTangent;
layout (location = 5) in vec3 inColor;

layout (location = 6) in vec3 blockLocation;

layout (location = 7) in uint blockID;

uniform vec3[256] blockColors;

out VertexData {
    vec3 worldPosition;
    vec3 normal;
    vec2 texCoord;
    vec3 tangent;
    vec3 biTangent;
    vec3 color;
} o;

uniform mat4 VP;

void main()
{
    o.worldPosition = vec3(blockLocation + inPosition);
    o.normal = inNormal;
    o.texCoord = inTexCoord;
    o.tangent = inTangent;
    o.biTangent = inBiTangent;
    o.color = blockColors[blockID];

    gl_Position = VP * vec4(o.worldPosition, 1.0);
}