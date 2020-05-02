#pragma once

#include <string>
#include <vector>

unsigned int loadCubemap(std::vector<std::string> faces);

unsigned int loadDebugCubemap();

unsigned int loadSkyboxCubemap();

unsigned int TextureFromFile(std::string filename, bool gamma = false);