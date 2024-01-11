#include <Windows.h>
#include "GraphicsEngine.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,      // Handle to the current instance
    _In_opt_ HINSTANCE hPrevInstance,  // Handle to the previous instance (always NULL, but marked as optional)
    _In_ LPSTR lpCmdLine,          // Command line for the application
    _In_ int nCmdShow              // Controls how the window is to be shown
) {
    GraphicsEngine* graphicsEngine = new GraphicsEngine();
    if (!graphicsEngine->Initialize()) {
        delete graphicsEngine;  
        return 0;
    }

    graphicsEngine->Execute();

    delete graphicsEngine;
    graphicsEngine = nullptr;

    return 0;
}
