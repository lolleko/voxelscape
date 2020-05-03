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

#include "vs_drawable.h"
#include "vs_mesh.h"
#include "vs_shader.h"
#include "vs_vertex_context.h"
#include "vs_modelloader.h"

class VSModel : public IVSDrawable
{
public:
    std::vector<VSMesh> meshes;
    std::string directory;
    bool gammaCorrection;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    VSModel(std::string const& path, bool gamma = false)
        : gammaCorrection(gamma)
    {
        meshes = std::move(loadModel(path));
    }

    // draws the model, and thus all its meshes
    void draw(std::shared_ptr<VSWorld> world, std::shared_ptr<VSShader> shader) const override
    {
        for (const auto& mesh : meshes)
        {
            mesh.draw(world, shader);
        }
    }
};
