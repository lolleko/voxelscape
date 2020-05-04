#include "glm/glm.hpp"

#include <sstream>

struct VSUIState
{
    glm::vec4 clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool isWireframeModeEnabled = false;
    bool bShouldDrawChunkBorderblocks = false;
    int activeBlockCount = 0;
    int totalBlockCount = 0;
    std::ostringstream logStream;
};