#pragma once

#include <assimp/vector3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

#include "vs_mesh.h"
#include "vs_shader.h"
#include "vs_textureloader.h"

class VSModel
{
public:
    /*  Model Data */
    std::vector<VSTexture> textures_loaded;  // stores all the textures loaded so far, optimization
                                             // to make sure textures aren't loaded more than once.
    std::vector<VSMesh> meshes;
    std::string directory;
    bool gammaCorrection;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    VSModel(std::string const& path, bool gamma = false)
        : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void draw(VSShader* shader) const
    {
        for (auto& mesh : meshes)
        {
            mesh.draw(shader);
        }
    }

private:
    /*  Functions   */
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in
    // the meshes vector.
    void loadModel(std::string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode)  // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node
    // and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene.
            // the scene contains all the data, node is just to keep stuff organized (like relations
            // between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the
        // children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    VSMesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // the following only works if assimp and glm vector have the same size
        assert(sizeof(glm::vec3) == sizeof(aiVector3D));

        std::vector<glm::vec3> vertexPositions(
            (glm::vec3*)mesh->mVertices, (glm::vec3*)(mesh->mVertices + mesh->mNumVertices));

        std::vector<glm::vec3> vertexNormals(
            (glm::vec3*)mesh->mNormals, (glm::vec3*)(mesh->mNormals + mesh->mNumVertices));

        std::vector<glm::vec2> vertexTexCoords;
        vertexTexCoords.reserve(mesh->mNumVertices);

        if (mesh->mTextureCoords[0])  // does the mesh contain texture coordinates?
        {
            std::for_each(
                mesh->mTextureCoords[0],
                mesh->mTextureCoords[0] + mesh->mNumVertices,
                [&vertexTexCoords](const aiVector3D& coord) {
                    vertexTexCoords.push_back(glm::vec2(coord.x, coord.y));
                });
        }
        else
        {
            vertexTexCoords.insert(
                vertexTexCoords.end(), mesh->mNumVertices, glm::vec2(0.0f, 0.0f));
        }

        std::vector<glm::vec3> vertexTangents(
            (glm::vec3*)mesh->mTangents, (glm::vec3*)(mesh->mTangents + mesh->mNumVertices));

        std::vector<glm::vec3> vertexBiTangents(
            (glm::vec3*)mesh->mBitangents, (glm::vec3*)(mesh->mBitangents + mesh->mNumVertices));

        // TODO default initialize for now
        // in future either read vertex colors or set to constant
        std::vector<glm::vec3> vertexColors(mesh->mNumVertices, glm::vec3(0.5, 0.5, 0.5));

        std::vector<GLuint> triangleIndices;

        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the
        // corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                triangleIndices.push_back(face.mIndices[j]);
            }
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be
        // named as 'texture_diffuseN' where N is a sequential number ranging from 1 to
        // MAX_SAMPLER_NUMBER. Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        std::vector<VSTexture> textures;

        // 1. diffuse maps
        std::vector<VSTexture> diffuseMaps =
            loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<VSTexture> specularMaps =
            loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<VSTexture> normalMaps =
            loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<VSTexture> heightMaps =
            loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a mesh object created from the extracted mesh data
        return VSMesh(
            vertexPositions,
            vertexNormals,
            vertexTexCoords,
            vertexTangents,
            vertexBiTangents,
            vertexColors,
            textures,
            triangleIndices);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded
    // yet. the required info is returned as a Texture struct.
    std::vector<VSTexture>
    loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<VSTexture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip
            // loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;  // a texture with the same filepath has already been loaded,
                                  // continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {  // if texture hasn't been loaded already, load it
                VSTexture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(
                    texture);  // store it as texture loaded for entire model, to ensure we won't
                               // unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};
