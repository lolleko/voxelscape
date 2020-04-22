#pragma once

struct VSUIState;
class GLFWwindow;

class VSUI
{
public:
    VSUI();

    void setup(const char* glsl_version, GLFWwindow* window);

    void cleanup();

    void render();

    void draw();

    const VSUIState* getState();

private:
    VSUIState* uiState;
};