#include <glad/glad.h>
#include <stb_image.h>
#include <algorithm>
#include <filesystem>

#include "core/vs_log.h"
#include "renderer/vs_textureloader.h"

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width;
    int height;
    int nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data);
            stbi_image_free(data);
        }
        else
        {
            VSLog::Log(
                VSLog::Category::Core,
                VSLog::Level::err,
                "Cubemap tex failed to load at path: {}",
                faces[i].c_str());
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int loadDebugCubemap()
{
    std::vector<std::string> faces{
        "resources/textures/cubemapDebug/right.jpg",
        "resources/textures/cubemapDebug/left.jpg",
        "resources/textures/cubemapDebug/top.jpg",
        "resources/textures/cubemapDebug/bottom.jpg",
        "resources/textures/cubemapDebug/front.jpg",
        "resources/textures/cubemapDebug/back.jpg"};
    return loadCubemap(faces);
}

unsigned int loadSkyboxCubemap()
{
    std::vector<std::string> faces{
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"};
    return loadCubemap(faces);
}

unsigned int TextureFromFile(std::string filename, bool gamma)
{
    (void)gamma;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width;
    int height;
    int nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        VSLog::Log(
            VSLog::Category::Core,
            VSLog::Level::err,
            "Texture failed to load at path: {}",
            filename.c_str());
        stbi_image_free(data);
    }

    return textureID;
};

unsigned int TextureAtlasFromFile(std::string atlasDir, bool gamma)
{
    (void)gamma;

    int width;
    int height;
    int nrComponents;
    GLenum format = GL_RGB;

    std::vector<unsigned char> pixels;
    int imageCount = 0;

    const auto dirIter = std::filesystem::directory_iterator(atlasDir);
    auto dirContents = std::vector<std::filesystem::directory_entry>(begin(dirIter), end(dirIter));

    std::sort(dirContents.begin(), dirContents.end());

    for (const auto& imagePath : dirContents)
    {
        unsigned char* data =
            stbi_load(imagePath.path().string().c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            pixels.insert(pixels.end(), data, data + (width * height * nrComponents));

            imageCount++;

            stbi_image_free(data);
        }
        else
        {
            VSLog::Log(
                VSLog::Category::Core,
                VSLog::Level::err,
                "TextureAtlas failed to load at path: {}",
                imagePath.path().string().c_str());
            stbi_image_free(data);
        }
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        format,
        width,
        height,
        imageCount,
        0,
        format,
        GL_UNSIGNED_BYTE,
        pixels.data());

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
};

unsigned int TextureFromData(unsigned char* data, int width, int height, int nrComponents)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        VSLog::Log(VSLog::Category::Core, VSLog::Level::err, "Data pointer is null");
    }

    return textureID;
}