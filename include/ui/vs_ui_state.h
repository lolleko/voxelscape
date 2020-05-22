#include <glm/ext/vector_float4.hpp>
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
    bool bShouldFreezeFrustum = false;
    int activeBlockCount = 0;
    int totalBlockCount = 0;
    std::ostringstream logStream;
};