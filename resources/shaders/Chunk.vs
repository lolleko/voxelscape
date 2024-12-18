#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

layout (location = 2) in vec3 blockLocation;

layout (location = 3) in uint blockID;

layout (location = 4) in uint inLight;

uniform vec3 origin;

out VertexData {
    vec3 worldPosition;
    vec3 normal;
    vec2 texCoord;
    flat uint blockID;
    vec3 vertexLight;
    float ao;
} o;

uniform mat4 VP;

uint getByte(in uint num, in uint n)
{
    return (num & (0x000000FFu << (n * 8u))) >> (n * 8u);
}

uint unpack2bit(in uint num, in uint startBit)
{
    return (num & (0x0000003u << (startBit))) >> (startBit);
}

uint unpack10bit(in uint num, in uint startBit)
{
    return (num & (0x000003FFu << (startBit))) >> (startBit);
}

vec3 getLight(in vec3 faceNormal, inout vec3 vertexPos, inout vec2 texCoord)
{
    if (faceNormal.x == 1) {
        texCoord.x = vertexPos.z + 0.5;
        texCoord.y = vertexPos.y + 0.5;
    }
    if (faceNormal.x == -1) {
        texCoord.x = vertexPos.z + 0.5;
        texCoord.y = vertexPos.y + 0.5;
    }
    if (faceNormal.y == 1) {
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.z + 0.5;
    }
    if (faceNormal.y == -1) {
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.z + 0.5;
    }
    if (faceNormal.z == 1) {
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.y + 0.5;
    }
    if (faceNormal.z == -1) {
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.y + 0.5;
    }

    uvec3 lightLevelDeNorm = uvec3(unpack10bit(inLight, 2u), unpack10bit(inLight, 12u), unpack10bit(inLight, 22u));

    return lightLevelDeNorm / 1023.0;
}

void main()
{
    // getLight might require us to flip teh quad
    vec3 vertexPosition = inPosition;
    vec2 texCoord = vec2(0);
    vec3 vertexLight = getLight(inNormal, vertexPosition, texCoord);

    o.worldPosition = origin + vec3(blockLocation + vertexPosition);
    o.normal = inNormal;
    o.texCoord = texCoord;
    o.blockID = blockID;
    o.vertexLight = vertexLight;
    o.ao = unpack2bit(inLight, 0u) / 3.0;

    gl_Position = VP * vec4(o.worldPosition, 1.0);
}