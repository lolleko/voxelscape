#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

class VSShader
{
public:
    VSShader(const char* name)
    {
        const auto vertexDir = std::filesystem::path(shaderDirectory) / "vertex";
        const auto fragmentDir = std::filesystem::path(shaderDirectory) / "fragment";

        const auto vertexShaderPath = (vertexDir / name).replace_extension(".glsl");

        ID = glCreateProgram();

        GLuint vertexShaderID = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
        glAttachShader(ID, vertexShaderID);

        const auto fragmentShaderPath = (fragmentDir / vertexShaderPath.filename());

        auto hasFragmentShader = false;
        GLuint fragmentShaderID = -1;
        if (!std::filesystem::exists(fragmentShaderPath))
        {
            std::cout << "Vertex shader: " << vertexShaderPath
                      << " is present, but corresponding fragment shader: " << fragmentShaderPath
                      << " is missing\n!";
        }
        else
        {
            fragmentShaderID = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
            glAttachShader(ID, fragmentShaderID);
        }
        glLinkProgram(ID);

        checkCompileErrors(ID, GL_PROGRAM);

        glDetachShader(ID, vertexShaderID);
        glDetachShader(ID, fragmentShaderID);

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
    }

    void use() const
    {
        glUseProgram(ID);
    }

    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const std::string& name, GLint value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec3(const std::string& name, glm::vec3 value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setMat4(const std::string& name, glm::mat4 value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }

private:
    GLuint ID;

    inline static const auto shaderDirectory = std::filesystem::path("shaders");
    inline static const auto vertexDir = shaderDirectory / "vertex";
    inline static const auto fragmentDir = shaderDirectory / "fragment";

    static void checkCompileErrors(unsigned int shader, GLenum type)
    {
        int success = 1;
        int infoLogLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> infoLog(infoLogLength + 1);

        if (type != GL_PROGRAM)
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
        }
        if (success == 0)
        {
            std::string typeString = type == GL_SHADER ? "Shader" : "program";

            glGetShaderInfoLog(shader, 1024, nullptr, &infoLog[0]);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << typeString << "\n"
                      << &infoLog[0]
                      << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }

    static GLuint compileShader(const std::filesystem::path& shaderPath, GLenum shaderType)
    {
        std::ifstream shaderStream(shaderPath);
        std::string shaderString(
            (std::istreambuf_iterator<char>(shaderStream)), std::istreambuf_iterator<char>());

        std::cout << "Compiling: " << shaderPath << "\n";

        GLuint shaderID = glCreateShader(shaderType);

        // Compile Vertex Shader
        char const* shaderSourcePointer = shaderString.c_str();
        glShaderSource(shaderID, 1, &shaderSourcePointer, nullptr);
        glCompileShader(shaderID);

        checkCompileErrors(shaderID, GL_SHADER);
        return shaderID;
    }
};
