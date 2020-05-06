#pragma once

#include <assimp/vector3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

class VSModel : public IVSDrawable
{
public:
    VSModel(std::string const& path, bool gamma = false);

    void draw(std::shared_ptr<VSWorld> world, std::shared_ptr<VSShader> shader) const override;

private:
    std::vector<VSMesh> meshes;
    std::string directory;
    bool gammaCorrection;

};
