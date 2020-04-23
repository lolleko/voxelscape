#include "glm/glm.hpp"

struct VSUIState
{
    glm::vec3 cameraPos = glm::vec3(4.f, 3.f, 3.f);
    glm::vec4 clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
    glm::vec3 lightPos = glm::vec3(4.f, 3.f, 3.f);
    glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);
};