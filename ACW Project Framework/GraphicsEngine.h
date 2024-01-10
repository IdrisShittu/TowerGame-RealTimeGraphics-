#pragma once
#include <Windows.h>
#include <memory>
#include "InputManager.h"
#include "GraphicsRenderer.h"

class GraphicsEngine {
public:
    GraphicsEngine();
    ~GraphicsEngine();
    GraphicsEngine(const GraphicsEngine&) = delete;
    GraphicsEngine& operator=(const GraphicsEngine&) = delete;
    GraphicsEngine(GraphicsEngine&&) noexcept = delete;
    GraphicsEngine& operator=(GraphicsEngine&&) noexcept = delete;

    bool Initialize();
    void Execute();
    void Shutdown();

    LRESULT MessageHandler(HWND const hwnd, UINT const umsg, WPARAM const wparam, LPARAM const lparam);

private:
    bool Update();
    bool InitializeWindows(int& screenWidth, int& screenHeight);
    void ShutdownWindows();

    HINSTANCE hInstance;
    HWND hwnd;
    LPCSTR appName;

    std::unique_ptr<InputManager> input;
    std::unique_ptr<GraphicsRenderer> graphics;
};

static LRESULT CALLBACK WndProc(HWND const hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

//Global
static GraphicsEngine* applicationHandle = nullptr;