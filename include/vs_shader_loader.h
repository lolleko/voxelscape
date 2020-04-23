#pragma once

#include "glad/glad.h"

#include <map>
#include <string>
#include <filesystem>

std::map<std::string, GLuint> VSLoadShaders(const char* shaderDirectory);

GLuint VSCompileShader(std::filesystem::path shaderPath, GLenum shaderType);