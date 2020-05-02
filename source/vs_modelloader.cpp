#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "vs_modelloader.h"
#include "vs_textureloader.h"

std::vector<VSMesh> loadModel(std::string const& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return {};
    }
    std::vector<VSMesh> meshes;
    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, meshes);

    return meshes;
}

std::unique_ptr<VSVertexContext> loadVertexContext(std::string const& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return {};
    }
    std::vector<VSMesh> meshes;
    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, meshes);

    return processMeshVertices(scene->mMeshes[scene->mRootNode->mMeshes[0]]);
}

void processNode(aiNode* node, const aiScene* scene, std::vector<VSMesh>& outMeshes)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations
        // between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        outMeshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the
    // children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, outMeshes);
    }
}

std::unique_ptr<VSVertexContext> processMeshVertices(aiMesh*& mesh)
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
        vertexTexCoords.insert(vertexTexCoords.end(), mesh->mNumVertices, glm::vec2(0.0f, 0.0f));
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

    return std::make_unique<VSVertexContext>(
        vertexPositions,
        vertexNormals,
        vertexTexCoords,
        vertexTangents,
        vertexBiTangents,
        vertexColors,
        triangleIndices);
}

VSMesh processMesh(aiMesh* mesh, const aiScene* scene)
{
    auto vertexContext = processMeshVertices(mesh);

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
    return VSMesh(std::move(vertexContext), textures);
}

static inline std::vector<VSTexture> g_textures_loaded;

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
        for (unsigned int j = 0; j < g_textures_loaded.size(); j++)
        {
            if (std::strcmp(g_textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(g_textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {  // if texture hasn't been loaded already, load it
            VSTexture texture;
            texture.id = TextureFromFile(str.C_Str());
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            g_textures_loaded.push_back(
                texture);  // store it as texture loaded for entire model, to ensure we won't
                           // unnecesery load duplicate textures.
        }
    }
    return textures;
}