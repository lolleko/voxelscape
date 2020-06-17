#pragma once

#include <glad/glad.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

#include "core/vs_log.h"

class VSShader
{
public:
    // TODO move to cpp and/or seperate class
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

        VSShaderUniformProxy& setUVec3(const std::string& name, glm::uvec3 value)
        {
            glUniform3uiv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
            return *this;
        }

        VSShaderUniformProxy& setMat4(const std::string& name, glm::mat4 value)
        {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
            return *this;
        }

        VSShaderUniformProxy&
        setVec3Array(const std::string& name, const std::vector<glm::vec3>& values)
        {
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), values.size(), &values[0][0]);
            return *this;
        }

    private:
        GLuint ID;
    };

    VSShader(const char* name, bool bIsComputeShader = false);

    [[nodiscard]] GLuint getID() const;

    void use() const;

    [[nodiscard]] VSShaderUniformProxy uniforms() const;

private:
    GLuint ID;

    inline static const auto shaderDirectory = std::filesystem::path("shaders");

    static bool checkShaderCompileErrors(unsigned int shaderID);

    static bool checkProgramLinkErrors(unsigned int programID);

    static GLuint compileShader(const std::filesystem::path& shaderPath, GLenum shaderType);
};