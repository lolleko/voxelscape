#pragma once
#include <filesystem>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/ext/vector_int2.hpp>
#include <sstream>

struct VSUIState
{
    glm::vec4 clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool isWireframeModeEnabled = false;
    bool bShouldDrawChunkBorder = false;

    int activeBlockCount = 0;
    bool bShouldFreezeFrustum = false;
    bool bAreShadowsEnabled = false;
    bool bIsAmbientOcclusionEnabled = true;
    bool bShouldShowAO = false;
    bool bShouldShowUV = false;
    bool bShouldShowNormals = false;
    bool bShouldShowLight = false;
    int totalBlockCount = 0;
    int visibleBlockCount = 0;
    int drawnBlockCount = 0;
    int drawCallCount = 0;
    std::ostringstream logStream;
    glm::vec3 directLightDir = {-0.4F, 0.7F, -0.6F};

    bool anyWindowHovered = false;
};