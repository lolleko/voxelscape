#include "core/vs_app.h"

int main(int, char**)
{
    VSApp app;

    const auto initError = app.initialize();
    if (initError != 0) {
        return initError;
    }

    return app.mainLoop();
}
