#include "glm/glm.hpp"

#include <sstream>

struct VSUIState
{
    glm::vec4 clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
    glm::vec3 lightPos = glm::vec3(4.f, 3.f, 3.f);
    glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);
    bool isWireframeModeEnabled = false;
    std::ostringstream logStream;
};