#include <filesystem>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/ext/vector_int2.hpp>
#include <sstream>
#include "ui/vs_minimap.h"

struct VSUIState
{
    glm::vec4 clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool isWireframeModeEnabled = false;
    bool bShouldDrawChunkBorder = false;
    glm::ivec3 chunkSize = {32, 64, 32};
    glm::ivec2 chunkCount = {4, 4};
    bool bShouldUpdateChunks = false;
    bool bShouldGenerateTerrain = false;
    bool bShouldSetGameActive = false;

    int activeBlockCount = 0;
    bool bShouldFreezeFrustum = false;
    bool bAreShadowsEnabled = false;
    bool bIsAmbientOcclusionEnabled = true;
    int totalBlockCount = 0;
    int visibleBlockCount = 0;
    int drawnBlockCount = 0;
    int drawCallCount = 0;
    std::ostringstream logStream;
    glm::vec3 directLightDir = {-0.4F, 0.7F, -0.6F};

    // Editor Control flow
    bool bShouldSetEditorActive = false;
    bool bShouldResetEditor = false;
    bool bEditorActive = false;
    bool bShouldLoadFromFile = false;
    // This is so the keyboard input does not get processed if there is a file browser active
    bool bFileBrowserActive = false;
    std::filesystem::path loadFilePath = "";
    bool bShouldSaveToFile = false;
    bool bShouldSaveBuilding = false;
    std::filesystem::path saveFilePath = "";
    std::filesystem::path saveBuildingPath = "";
    bool bShouldUpdateBlockID = false;
    int bSetBlockID = 0;
    int bBiomeType = 0;

    // Menu Control flow
    bool bMenuActive = true;
    bool bGameConfigActive = false;

    // Game config
    int worldSize = 0;  // 0 = Small, 1 = Medium, 2 = Large
    // TODO: Not to sure if this belongs here
    VSMinimap* minimap = new VSMinimap();
    bool minimapChanged = false;
    unsigned int minimapTexture = 0;
};