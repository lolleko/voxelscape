#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

layout (location = 2) in vec3 blockLocation;

layout (location = 3) in uint blockID;

layout (location = 4) in uvec3 lightRight;
layout (location = 5) in uvec3 lightLeft;
layout (location = 6) in uvec3 lightTop;
layout (location = 7) in uvec3 lightBottom;
layout (location = 8) in uvec3 lightFront;
layout (location = 9) in uvec3 lightBack;

uniform vec3 origin;

out VertexData {
    vec3 worldPosition;
    vec3 normal;
    vec2 texCoord;
    flat uint blockID;
    vec3 vertexLight;
} o;

uniform mat4 VP;

uint getByte(in uint num, in uint n)
{
    return (num & (0x000000FFu << (n * 8u))) >> (n * 8u);
}

uvec3 getByteVec(in uvec3 num, in uint n)
{
    return uvec3(getByte(num[0], n), getByte(num[1], n), getByte(num[2], n));
}

uint getMaxByteInVec(in uvec3 num, in uint n)
{
    return max(max(getByte(num[0], n), getByte(num[1], n)), getByte(num[2], n));
}

vec3 getLight(in vec3 faceNormal, inout vec3 vertexPos, inout vec2 texCoord)
{
    uvec3 lightLevelDeNorm = uvec3(0, 0, 0);
    if (faceNormal.x == 1) {
        if (getMaxByteInVec(lightRight, 0u) + getMaxByteInVec(lightRight, 3u) > getMaxByteInVec(lightRight, 1u) + getMaxByteInVec(lightRight, 2u)) {
            float y = vertexPos.y;
            vertexPos.y = vertexPos.z;
            vertexPos.z = -y;
        }
        texCoord.x = vertexPos.z + 0.5;
        texCoord.y = vertexPos.y + 0.5;
        lightLevelDeNorm = getByteVec(lightRight, uint((vertexPos.y + 0.5) * 1 + (vertexPos.z + 0.5) * 2));
    }
    if (faceNormal.x == -1) {
        if (getMaxByteInVec(lightLeft, 0u) + getMaxByteInVec(lightLeft, 3u) > getMaxByteInVec(lightLeft, 1u) + getMaxByteInVec(lightLeft, 2u)) {
            float y = vertexPos.y;
            vertexPos.y = vertexPos.z;
            vertexPos.z = -y;
        }
        texCoord.x = vertexPos.z + 0.5;
        texCoord.y = vertexPos.y + 0.5;
        lightLevelDeNorm = getByteVec(lightLeft, uint((vertexPos.y + 0.5) * 1 + (vertexPos.z + 0.5) * 2));
    }
    if (faceNormal.y == 1) {
        if (getMaxByteInVec(lightTop, 0u) + getMaxByteInVec(lightTop, 3u) > getMaxByteInVec(lightTop, 1u) + getMaxByteInVec(lightTop, 2u)) {
            float x = vertexPos.x;
            vertexPos.x = vertexPos.z;
            vertexPos.z = -x;
        }
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.z + 0.5;
        lightLevelDeNorm = getByteVec(lightTop, uint((vertexPos.x + 0.5) * 1 + (vertexPos.z + 0.5) * 2));
    }
    if (faceNormal.y == -1) {
        if (getMaxByteInVec(lightBottom, 0u) + getMaxByteInVec(lightBottom, 3u) > getMaxByteInVec(lightBottom, 1u) + getMaxByteInVec(lightBottom, 2u)) {
            float x = vertexPos.x;
            vertexPos.x = vertexPos.z;
            vertexPos.z = -x;
        }
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.z + 0.5;
        lightLevelDeNorm = getByteVec(lightBottom, uint((vertexPos.x + 0.5) * 1 + (vertexPos.z + 0.5) * 2));
    }
    if (faceNormal.z == 1) {
        if (getMaxByteInVec(lightFront, 0u) + getMaxByteInVec(lightFront, 3u) > getMaxByteInVec(lightFront, 1u) + getMaxByteInVec(lightFront, 2u)) {
            float x = vertexPos.x;
            vertexPos.x = vertexPos.y;
            vertexPos.y = -x;
        }
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.y + 0.5;
        lightLevelDeNorm = getByteVec(lightFront, uint((vertexPos.x + 0.5) * 1 + (vertexPos.y + 0.5) * 2));
    }
    if (faceNormal.z == -1) {
        if (getMaxByteInVec(lightBack, 0u) + getMaxByteInVec(lightBack, 3u) > getMaxByteInVec(lightBack, 1u) + getMaxByteInVec(lightBack, 2u)) {
            float x = vertexPos.x;
            vertexPos.x = vertexPos.y;
            vertexPos.y = -x;
        }
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.y + 0.5;
        lightLevelDeNorm = getByteVec(lightBack, uint((vertexPos.x + 0.5) * 1 + (vertexPos.y + 0.5) * 2));
    }
    return lightLevelDeNorm / 255.0;
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

    gl_Position = VP * vec4(o.worldPosition, 1.0);
}