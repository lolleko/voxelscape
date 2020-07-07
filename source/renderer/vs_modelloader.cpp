#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <iostream>

#include "core/vs_log.h"
#include "renderer/vs_modelloader.h"
#include "renderer/vs_textureloader.h"

VSVertexContext* loadVertexContext(std::string const& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  // if is Not Zero
    {
        VSLog::Log(
            VSLog::Category::Resource,
            VSLog::Level::err,
            "{0}",
            std::string("ERROR::ASSIMP:: ") + importer.GetErrorString());
        return {};
    }

    return processMeshVertices(scene->mMeshes[0]);
}

VSVertexContext* processMeshVertices(aiMesh*& mesh)
{
    // the following only works if assimp and glm vector have the same size
    assert(sizeof(glm::vec3) == sizeof(aiVector3D));

    std::vector<VSVertexData> vertexDataList;

    for (std::size_t i = 0; i < mesh->mNumVertices; i++)
    {
        VSVertexData currentVertex{};
        currentVertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        currentVertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

        vertexDataList.emplace_back(currentVertex);
    }

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

    return new VSVertexContext(vertexDataList, triangleIndices);
}