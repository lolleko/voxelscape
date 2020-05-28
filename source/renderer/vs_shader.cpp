#include "renderer/vs_shader.h"

VSShader::VSShader(const char* name, bool bIsComputeShader)
{
    const auto vertexShaderPath = (shaderDirectory / name).replace_extension(".vs");

    ID = glCreateProgram();

    if (bIsComputeShader) {
        GLuint computeShaderID = compileShader(vertexShaderPath, GL_COMPUTE_SHADER);
        glAttachShader(ID, computeShaderID);

        glLinkProgram(ID);

        checkProgramLinkErrors(ID);

        glDetachShader(ID, computeShaderID);
        glDeleteShader(computeShaderID);
    } else {
        GLuint vertexShaderID = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
        glAttachShader(ID, vertexShaderID);

        const auto fragmentShaderPath = (shaderDirectory / name).replace_extension(".fs");

        GLuint fragmentShaderID = -1;
        if (!std::filesystem::exists(fragmentShaderPath))
        {
            VSLog::Log(
                VSLog::Category::Shader,
                VSLog::Level::warn,
                "Vertex shader: {} is present, but corresponding fragment shader: % is missing",
                vertexShaderPath.string(),
                fragmentShaderPath.string());
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
}

GLuint VSShader::getID() const
{
    return ID;
}

void VSShader::use() const
{
    glUseProgram(ID);
}

VSShader::VSShaderUniformProxy VSShader::uniforms() const
{
    return VSShaderUniformProxy(ID);
}

bool VSShader::checkShaderCompileErrors(unsigned int shaderID)
{
    int success = 1;
    int infoLogLength = 0;

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::vector<char> infoLog(infoLogLength + 1);

    if (success == 0)
    {
        glGetShaderInfoLog(shaderID, infoLogLength, nullptr, &infoLog[0]);
        VSLog::Log(
            VSLog::Category::Shader,
            VSLog::Level::critical,
            "Shader compilation failed:\n{}",
            std::string(infoLog.data()));
    }

    return success == 0;
}

bool VSShader::checkProgramLinkErrors(unsigned int programID)
{
    int success = 1;
    int infoLogLength = 0;

    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::vector<char> infoLog(infoLogLength + 1);

    if (success == 0)
    {
        glGetProgramInfoLog(programID, infoLogLength, nullptr, &infoLog[0]);
        VSLog::Log(
            VSLog::Category::Shader,
            VSLog::Level::critical,
            "Progam linking failed:\n{}",
            std::string(infoLog.data()));
    }

    return success == 0;
}

GLuint VSShader::compileShader(const std::filesystem::path& shaderPath, GLenum shaderType)
{
    std::ifstream shaderStream(shaderPath);
    std::string shaderString(
        (std::istreambuf_iterator<char>(shaderStream)), std::istreambuf_iterator<char>());

    VSLog::Log(
        VSLog::Category::Shader, VSLog::Level::info, "Compiling shader: {}", shaderPath.string());

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
            shaderPath.string());
    }

    return shaderID;
}
