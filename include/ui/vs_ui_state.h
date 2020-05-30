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
    glm::ivec3 chunkSize = {32, 64, 32};
    glm::ivec2 chunkCount = {2, 2};
    bool bShouldUpdateChunks = false;
    bool bShouldGenerateHeightMap = false;
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
    std::filesystem::path saveFilePath = "";
    int bSetBlockID = 1;

    // Menu Control flow
    bool bMenuActive = true;
};