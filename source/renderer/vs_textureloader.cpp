#include <glad/glad.h>
#include <stb_image.h>

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
    std::vector<std::string> faces{"textures/cubemapDebug/right.jpg",
                                   "textures/cubemapDebug/left.jpg",
                                   "textures/cubemapDebug/top.jpg",
                                   "textures/cubemapDebug/bottom.jpg",
                                   "textures/cubemapDebug/front.jpg",
                                   "textures/cubemapDebug/back.jpg"};
    return loadCubemap(faces);
}

unsigned int loadSkyboxCubemap()
{
    std::vector<std::string> faces{"textures/skybox/right.jpg",
                                   "textures/skybox/left.jpg",
                                   "textures/skybox/top.jpg",
                                   "textures/skybox/bottom.jpg",
                                   "textures/skybox/front.jpg",
                                   "textures/skybox/back.jpg"};
    return loadCubemap(faces);
}

unsigned int TextureFromFile(std::string filename, bool gamma)
{
    (void) gamma;

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