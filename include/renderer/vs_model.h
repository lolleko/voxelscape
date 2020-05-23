#pragma once

#include <assimp/vector3.h>
#include <glad/glad.h>

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

#include "renderer/vs_drawable.h"
#include "renderer/vs_mesh.h"
#include "renderer/vs_shader.h"
#include "renderer/vs_vertex_context.h"
#include "renderer/vs_modelloader.h"

// Roughly based on https://learnopengl.com/Model-Loading/Model

class VSModel : public IVSDrawable
{
public:
    VSModel(std::string const& path);

    void draw(VSWorld* world) override;

private:
    VSShader modelShader = VSShader("Monkey");  // TODO rename to model

    std::vector<VSMesh> meshes;
    std::string directory;
};
