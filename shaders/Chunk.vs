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
    vec2 texCoord;
    vec3 tangent;
    vec3 biTangent;
    vec3 color;
} o;

uniform mat4 MVP;
uniform mat4 model;

uniform vec3 chunkSize;

void main()
{
    int W = int(chunkSize.x);
    int D = int(chunkSize.z);
    int H = int(chunkSize.y);
    int Y = gl_InstanceID / (W * D);
    int Z = (gl_InstanceID - Y * W * D) / D;
    int X = (gl_InstanceID - Y * W * D) % D;

    gl_Position = MVP * vec4(inPosition + vec3(X, Y, Z), 1.0);

    o.worldPosition = vec3(model * vec4(inPosition, 1.0));
    o.normal = vec3(model * vec4(inNormal, 0.0));;
    o.texCoord = inTexCoord;
    o.tangent = vec3(model * vec4(inTangent, 0.0));;
    o.biTangent = vec3(model * vec4(inBiTangent, 0.0));;
    o.color = inColor;
}