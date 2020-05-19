#pragma once

#include <assimp/material.h>

#include <vector>
#include <filesystem>

#include "vs_mesh.h"
#include "vs_vertex_context.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

std::vector<VSMesh> loadModel(std::string const& path);

VSVertexContext* loadVertexContext(std::string const& path);

void processNode(aiNode* node, const aiScene* scene, std::vector<VSMesh>& OutMeshes);

VSMesh processMesh(aiMesh* mesh, const aiScene* scene);

VSVertexContext* processMeshVertices(aiMesh*& mesh);

std::vector<VSTexture>
loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);