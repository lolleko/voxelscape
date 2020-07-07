#pragma once

#include <assimp/material.h>

#include <vector>
#include <filesystem>

#include "vs_vertex_context.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

VSVertexContext* loadVertexContext(std::string const& path);

VSVertexContext* processMeshVertices(aiMesh*& mesh);
