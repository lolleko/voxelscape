#include "vs_shader_loader.h"

std::map<std::string, GLuint> VSLoadShaders(const char* shaderDirectory)
{
    auto shaderNameToProgramID = std::map<std::string, GLuint>();

    const auto vertexDir = std::filesystem::path(shaderDirectory) / "vertex";
    const auto fragmentDir = std::filesystem::path(shaderDirectory) / "fragment";

    for (const auto& vertexShaderPath : std::filesystem::directory_iterator(vertexDir))
    {
        GLuint programID = glCreateProgram();
        GLuint vertexShaderID = VSCompileShader(vertexShaderPath, GL_VERTEX_SHADER);
        glAttachShader(programID, vertexShaderID);

        const auto fragmentShaderPath = (fragmentDir / vertexShaderPath.path().filename());

        auto hasFragmentShader = false;
        GLuint fragmentShaderID = -1;
        if (!std::filesystem::exists(fragmentShaderPath))
        {
            std::cout << "Vertex shader: " << vertexShaderPath.path()
                      << " is present, but corresponding fragment shader: " << fragmentShaderPath
                      << " is missing\n!";
        }
        else
        {
            fragmentShaderID = VSCompileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
            glAttachShader(programID, fragmentShaderID);
        }

        glLinkProgram(programID);

        GLint result = GL_FALSE;
        int infoLogLength;

        // Check the program
        glGetProgramiv(programID, GL_LINK_STATUS, &result);
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            std::vector<char> ProgramErrorMessage(infoLogLength + 1);
            glGetProgramInfoLog(programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }

        glDetachShader(programID, vertexShaderID);
        glDetachShader(programID, fragmentShaderID);

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);

        shaderNameToProgramID.insert(
            {vertexShaderPath.path().filename().replace_extension().string(), programID});
    }
    return shaderNameToProgramID;
}

GLuint VSCompileShader(std::filesystem::path shaderPath, GLenum shaderType)
{
    std::ifstream shaderStream(shaderPath);
    std::string shaderString(
        (std::istreambuf_iterator<char>(shaderStream)), std::istreambuf_iterator<char>());

    std::cout << "Compiling: " << shaderPath << "\n";

    GLuint shaderID = glCreateShader(shaderType);

    GLint compileResult = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    char const* shaderSourcePointer = shaderString.c_str();
    glShaderSource(shaderID, 1, &shaderSourcePointer, nullptr);
    glCompileShader(shaderID);

    // Check Vertex Shader
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileResult);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(shaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    return shaderID;
}