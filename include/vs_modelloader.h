#pragma once

#include <glm/glm.hpp>

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <filesystem>

#include "vs_mesh.h"
#include "vs_textureloader.h"
#include "vs_vertex_context.h"

std::vector<VSMesh> loadModel(std::string const& path);
std::unique_ptr<VSVertexContext> loadVertexContext(std::string const& path);
void processNode(aiNode* node, const aiScene* scene, std::vector<VSMesh>& OutMeshes);
VSMesh processMesh(aiMesh* mesh, const aiScene* scene);
std::unique_ptr<VSVertexContext> processMeshVertices(aiMesh*& mesh);
std::vector<VSTexture>
loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);