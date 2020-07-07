#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

layout (location = 2) in vec3 blockLocation;

layout (location = 3) in uint blockID;

layout (location = 4) in uint lightRight;
layout (location = 5) in uint lightLeft;
layout (location = 6) in uint lightTop;
layout (location = 7) in uint lightBottom;
layout (location = 8) in uint lightFront;
layout (location = 9) in uint lightBack;

uniform vec3[7] blockColors;

uniform vec3 origin;

out VertexData {
    vec3 worldPosition;
    vec3 normal;
    vec2 texCoord;
    vec3 material;
    flat uint blockID;
    float lightLevel;
} o;

uniform mat4 VP;

uint getByte(in uint num, in uint n)
{
    return (num & (0x000000FFu << (n * 8u))) >> (n * 8u);
}

float getLight(in vec3 faceNormal, inout vec3 vertexPos, inout vec2 texCoord)
{
    float lightLevelDeNorm = 0.0;
    if (faceNormal.x == 1) {
        if (getByte(lightRight, 0u) + getByte(lightRight, 3u) > getByte(lightRight, 1u) + getByte(lightRight, 2u)) {
            float y = vertexPos.y;
            vertexPos.y = vertexPos.z;
            vertexPos.z = -y;
        }
        texCoord.x = vertexPos.z + 0.5;
        texCoord.y = vertexPos.y + 0.5;
        lightLevelDeNorm = getByte(lightRight, uint((vertexPos.y + 0.5) * 1 + (vertexPos.z + 0.5) * 2));
    }
    if (faceNormal.x == -1) {
        if (getByte(lightLeft, 0u) + getByte(lightLeft, 3u) > getByte(lightLeft, 1u) + getByte(lightLeft, 2u)) {
            float y = vertexPos.y;
            vertexPos.y = vertexPos.z;
            vertexPos.z = -y;
        }
        texCoord.x = vertexPos.z + 0.5;
        texCoord.y = vertexPos.y + 0.5;
        lightLevelDeNorm = getByte(lightLeft, uint((vertexPos.y + 0.5) * 1 + (vertexPos.z + 0.5) * 2));
    }
    if (faceNormal.y == 1) {
        if (getByte(lightTop, 0u) + getByte(lightTop, 3u) > getByte(lightTop, 1u) + getByte(lightTop, 2u)) {
            float x = vertexPos.x;
            vertexPos.x = vertexPos.z;
            vertexPos.z = -x;
        }
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.z + 0.5;
        lightLevelDeNorm = getByte(lightTop, uint((vertexPos.x + 0.5) * 1 + (vertexPos.z + 0.5) * 2));
    }
    if (faceNormal.y == -1) {
        if (getByte(lightBottom, 0u) + getByte(lightBottom, 3u) > getByte(lightBottom, 1u) + getByte(lightBottom, 2u)) {
            float x = vertexPos.x;
            vertexPos.x = vertexPos.z;
            vertexPos.z = -x;
        }
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.z + 0.5;
        lightLevelDeNorm = getByte(lightBottom, uint((vertexPos.x + 0.5) * 1 + (vertexPos.z + 0.5) * 2));
    }
    if (faceNormal.z == 1) {
        if (getByte(lightFront, 0u) + getByte(lightFront, 3u) > getByte(lightFront, 1u) + getByte(lightFront, 2u)) {
            float x = vertexPos.x;
            vertexPos.x = vertexPos.y;
            vertexPos.y = -x;
        }
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.y + 0.5;
        lightLevelDeNorm = getByte(lightFront, uint((vertexPos.x + 0.5) * 1 + (vertexPos.y + 0.5) * 2));
    }
    if (faceNormal.z == -1) {
        if (getByte(lightBack, 0u) + getByte(lightBack, 3u) > getByte(lightBack, 1u) + getByte(lightBack, 2u)) {
            float x = vertexPos.x;
            vertexPos.x = vertexPos.y;
            vertexPos.y = -x;
        }
        texCoord.x = vertexPos.x + 0.5;
        texCoord.y = vertexPos.y + 0.5;
        lightLevelDeNorm = getByte(lightBack, uint((vertexPos.x + 0.5) * 1 + (vertexPos.y + 0.5) * 2));
    }
    return lightLevelDeNorm / 255.0;
}

void main()
{
    // getLight might require us to flip teh quad
    vec3 vertexPosition = inPosition;
    vec2 texCoord = vec2(0);
    float lightLevel = getLight(inNormal, vertexPosition, texCoord);

    o.worldPosition = origin + vec3(blockLocation + vertexPosition);
    o.normal = inNormal;
    o.texCoord = texCoord;
    o.material = blockColors[blockID];
    o.blockID = blockID;
    o.lightLevel = lightLevel;

    gl_Position = VP * vec4(o.worldPosition, 1.0);
}