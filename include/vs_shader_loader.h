#pragma once

#include "glad/glad.h"

#include <map>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

std::map<std::string, GLuint> VSLoadShaders(const char* shaderDirectory);

GLuint VSCompileShader(std::filesystem::path shaderPath, GLenum shaderType);