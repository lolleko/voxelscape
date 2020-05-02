#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "vs_log.h"

class VSShader
{
public:
    class VSShaderUniformProxy
    {
    public:
        VSShaderUniformProxy(GLuint ID)
            : ID(ID)
        {
            glUseProgram(ID);
        };

        VSShaderUniformProxy& setBool(const std::string& name, bool value)
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
            return *this;
        }

        VSShaderUniformProxy& setInt(const std::string& name, GLint value)
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
            return *this;
        }

        VSShaderUniformProxy& setFloat(const std::string& name, float value)
        {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
            return *this;
        }

        VSShaderUniformProxy& setVec3(const std::string& name, glm::vec3 value)
        {
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
            return *this;
        }

        VSShaderUniformProxy& setMat4(const std::string& name, glm::mat4 value)
        {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
            return *this;
        }

    private:
        GLuint ID;
    };

    VSShader(const char* name)
    {
        const auto vertexShaderPath = (shaderDirectory / name).replace_extension(".vs");

        ID = glCreateProgram();

        GLuint vertexShaderID = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
        glAttachShader(ID, vertexShaderID);

        const auto fragmentShaderPath = (shaderDirectory / name).replace_extension(".fs");

        auto hasFragmentShader = false;
        GLuint fragmentShaderID = -1;
        if (!std::filesystem::exists(fragmentShaderPath))
        {
            VSLog::Log(
                VSLog::Category::Shader,
                VSLog::Level::warn,
                "Vertex shader: {} is present, but corresponding fragment shader: {} is missing",
                vertexShaderPath.c_str(),
                fragmentShaderPath.c_str());
        }
        else
        {
            fragmentShaderID = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
            glAttachShader(ID, fragmentShaderID);
        }
        glLinkProgram(ID);

        checkProgramLinkErrors(ID);

        glDetachShader(ID, vertexShaderID);
        glDetachShader(ID, fragmentShaderID);

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
    }

    GLuint getID() const
    {
        return ID;
    }

    void use() const
    {
        glUseProgram(ID);
    }

    VSShaderUniformProxy uniforms()
    {
        return VSShaderUniformProxy(ID);
    };

private:
    GLuint ID;

    inline static const auto shaderDirectory = std::filesystem::path("shaders");

    static bool checkShaderCompileErrors(unsigned int shaderID)
    {
        int success = 1;
        int infoLogLength = 0;

        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<char> infoLog(infoLogLength + 1);

        if (success == 0)
        {
            glGetShaderInfoLog(shaderID, 1024, nullptr, &infoLog[0]);
            VSLog::Log(
                VSLog::Category::Shader,
                VSLog::Level::critical,
                "Shader compilation failed:\n{}",
                &infoLog[0]);
        }

        return success == 0;
    }

    static bool checkProgramLinkErrors(unsigned int programID)
    {
        int success = 1;
        int infoLogLength = 0;

        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<char> infoLog(infoLogLength + 1);

        if (success == 0)
        {
            glGetProgramInfoLog(programID, 1024, nullptr, &infoLog[0]);
            VSLog::Log(
                VSLog::Category::Shader,
                VSLog::Level::critical,
                "Progam linking failed:\n{}",
                &infoLog[0]);
        }

        return success == 0;
    }

    static GLuint compileShader(const std::filesystem::path& shaderPath, GLenum shaderType)
    {
        std::ifstream shaderStream(shaderPath);
        std::string shaderString(
            (std::istreambuf_iterator<char>(shaderStream)), std::istreambuf_iterator<char>());

        VSLog::Log(
            VSLog::Category::Shader,
            VSLog::Level::info,
            "Compiling shader: {}",
            shaderPath.c_str());

        GLuint shaderID = glCreateShader(shaderType);

        // Compile Vertex Shader
        char const* shaderSourcePointer = shaderString.c_str();
        glShaderSource(shaderID, 1, &shaderSourcePointer, nullptr);
        glCompileShader(shaderID);

        const auto hadCompileError = checkShaderCompileErrors(shaderID);
        if (!hadCompileError)
        {
            VSLog::Log(
                VSLog::Category::Shader,
                VSLog::Level::info,
                "Succesfully compiled shader: {}",
                shaderPath.c_str());
        }

        return shaderID;
    }
};