#pragma once

#include <assimp/material.h>

#include <vector>
#include <filesystem>

#include "vs_vertex_context.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

VSMeshVertices loadMeshVertices(const std::string& path);

VSVertexContext* createVertexContext(const VSMeshVertices& meshVertices);
