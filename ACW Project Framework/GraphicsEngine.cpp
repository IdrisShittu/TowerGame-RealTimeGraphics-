#include "GraphicsEngine.h"

GraphicsEngine::GraphicsEngine() : hInstance(nullptr), hwnd(nullptr), appName("Rocket Simulation") {}

GraphicsEngine::~GraphicsEngine() {
	Shutdown();
}


bool GraphicsEngine::Initialize() {
	int screenWidth = 0;
	int screenHeight = 0;
	if (!InitializeWindows(screenWidth, screenHeight)) {
		return false;
	}

	input = std::make_unique<InputManager>();
	if (!input) {
		return false;
	}

	graphics = std::make_unique<GraphicsRenderer>(screenWidth, screenHeight, hwnd);
	if (!graphics) {
		return false;
	}

	return true;
}

void GraphicsEngine::Execute() {
	MSG msg = {};
	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				break;
			}
		}
		else {
			if (!Update()) {
				break;
			}
		}
	}
}



//This is where all the application processing happens, we check to see if the user wants to quit the application, if not then we call the graphics objects frame function which will render the graphics.
//More code will be added here as we add more functionality to the framework/ application.
bool GraphicsEngine::Update() {
	
	//Check if user wants to quit the application
	if (input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//For keys we limit to one execution on press we check if its up first
	if (input->IsKeyUp(0x52) && input->IsKeyUp(0x50) && input->IsKeyUp(0x54) && input->IsKeyUp(VK_F1) && input->IsKeyUp(VK_F2) && input->IsKeyUp(VK_F3) && input->IsKeyUp(VK_F4) && input->IsKeyUp(VK_F5) && input->IsKeyUp(VK_F6) && input->IsKeyUp(VK_F7) && input->IsKeyUp(VK_F11))
	{
		input->ToggleDoOnce(true);
	}

	//Reset simulation
	if (input->IsKeyDown(0x52) && input->DoOnce())
	{
		graphics->ResetToInitialState();
		input->ToggleDoOnce(false);
	}

	//Pause Simulation
	if (input->IsKeyDown(0x50) && input->DoOnce())
	{
		//graphics->TogglePauseSimulation();
		input->ToggleDoOnce(false);
	}

	//Launch Rocket
	if (input->IsKeyDown(VK_F11) && input->DoOnce())
	{
		graphics->LaunchRocket();
		input->ToggleDoOnce(false);
	}

	//Increase/Decrease TimeScale and rotate rocket
	if (input->IsKeyDown(0x10))
	{
		if (input->IsKeyDown(0x54) && input->DoOnce())
		{
			graphics->AddTimeScale(1);
			input->ToggleDoOnce(false);
		}

		if (input->IsKeyDown(VK_OEM_COMMA))
		{
			graphics->RotateRocketLeft();
		}

		if (input->IsKeyDown(VK_OEM_PERIOD))
		{
			graphics->RotateRocketRight();
		}
	}
	else
	{
		if (input->IsKeyDown(0x54) && input->DoOnce())
		{
			graphics->AddTimeScale(-1);
			input->ToggleDoOnce(false);
		}
	}

	//Change camera position
	if (input->IsKeyDown(VK_F1) && input->DoOnce())
	{
		graphics->ChangeCameraMode(0);
		input->ToggleDoOnce(false);
	}

	if (input->IsKeyDown(VK_F2) && input->DoOnce())
	{
		graphics->ChangeCameraMode(1);
		input->ToggleDoOnce(false);
	}

	if (input->IsKeyDown(VK_F3) && input->DoOnce())
	{
		graphics->ChangeCameraMode(2);
		input->ToggleDoOnce(false);
	}

	if (input->IsKeyDown(VK_F4) && input->DoOnce())
	{
		graphics->ChangeCameraMode(3);
		input->ToggleDoOnce(false);
	}

	if (input->IsKeyDown(VK_F5) && input->DoOnce())
	{
		graphics->ChangeCameraMode(4);
		input->ToggleDoOnce(false);
	}

	if (input->IsKeyDown(VK_F6) && input->DoOnce())
	{
		graphics->ToggleRenderOption();
		input->ToggleDoOnce(false);
	}

	if (input->IsKeyDown(VK_F7) && input->DoOnce())
	{
		graphics->ToggleOptionalGameObjects();
		input->ToggleDoOnce(false);
	}

	//Camera Controls
	if (input->IsKeyDown(VK_CONTROL))
	{
		if (input->IsKeyDown(0x57))
		{
			graphics->GetCamera()->AddPositionZ(0.1f);
		}

		if (input->IsKeyDown(0x53))
		{
			graphics->GetCamera()->AddPositionZ(-0.1f);
		}

		if (input->IsKeyDown(0x44))
		{
			graphics->GetCamera()->AddPositionX(0.1f);
		}

		if (input->IsKeyDown(0x41))
		{
			graphics->GetCamera()->AddPositionX(-0.1f);
		}

		if (input->IsKeyDown(VK_PRIOR))
		{
			graphics->GetCamera()->AddPositionY(0.1f);
		}

		if (input->IsKeyDown(VK_NEXT))
		{
			graphics->GetCamera()->AddPositionY(-0.1f);
		}
	}
	else
	{
		if (input->IsKeyDown(0x57))
		{
			//Rotate up
			graphics->GetCamera()->AddRotationY(-0.8f);
		}

		if (input->IsKeyDown(0x53))
		{
			//Rotate down
			graphics->GetCamera()->AddRotationY(0.8f);
		}

		if (input->IsKeyDown(0x44))
		{
			//Rotate right
			graphics->GetCamera()->AddRotationX(0.8f);
		}

		if (input->IsKeyDown(0x41))
		{
			//Rotate left
			graphics->GetCamera()->AddRotationX(-0.8f);
		}
	}


	//Call graphics objects update frame processing function
	auto const result = graphics->UpdateFrame();

	return result;
}

//Our MessageHandler where we direct the windows GraphicsEngine messages into. With this we can listen for certain information.
//For now we just read key presses and key releases and notifiy our input object, all other information we just pass back to the default windows message handler.
LRESULT CALLBACK GraphicsEngine::MessageHandler(HWND const hwnd, UINT const umsg, WPARAM const wparam, LPARAM const lparam) {
	
	switch(umsg)
	{
		//Check if a key has been pressed
		case WM_KEYDOWN:
		{
			// If a key is pressed then send it to our input object
			input->KeyDown(static_cast<unsigned int>(wparam));
			return 0;
		}

		//Check if a key has been released
		case WM_KEYUP:
		{
			//If a key is released then send it to our input object
			input->KeyUp(static_cast<unsigned int>(wparam));
			return 0;
		}

		//Send any other messages back to the default windows message handler
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

//This is where we build the window we are rendering to. We start by initializing a default window which can be full screen or a set size, depending on the global variable FULL_SCREEN in the GraphicsClass.h
bool GraphicsEngine::InitializeWindows(int& screenWidth, int& screenHeight) {
	
	WNDCLASSEX windowClass;
	DEVMODE deviceEnvironment;

	int positionX;
	int positionY;

	applicationHandle = this;
	hInstance = GetModuleHandle(nullptr);
	appName = "Rocket Simulation";

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = appName;
	windowClass.hIconSm = windowClass.hIcon;

	RegisterClassEx(&windowClass);
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		ZeroMemory(&deviceEnvironment, sizeof(deviceEnvironment));
		deviceEnvironment.dmSize = sizeof(deviceEnvironment);
		deviceEnvironment.dmPelsWidth = static_cast<unsigned long>(screenWidth);
		deviceEnvironment.dmPelsHeight = static_cast<unsigned long>(screenHeight);
		deviceEnvironment.dmBitsPerPel = 32;
		deviceEnvironment.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Change display settings to fullscreen using device environment
		ChangeDisplaySettings(&deviceEnvironment, CDS_FULLSCREEN);

	
		positionX = 0;
		positionY = 0;
	}
	else
	{
		screenWidth = 1360;
		screenHeight = 720;

		//Place window in the centre of the screen
		positionX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		positionY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//Create the window with the screen settings and get the handle to it
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, appName, appName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, positionX, positionY, screenWidth, screenHeight, nullptr, nullptr, hInstance, nullptr);

	//Show the window and bring it to the front of the applications
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//Hide the cursor
	ShowCursor(true);
	return true;
}

//Reverts the screen settings and releases the window class and any handles associated with it
void GraphicsEngine::ShutdownWindows() {
	
	//Show the cursor
	ShowCursor(true);

	//Fix display settings if we're in full screen mode
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	//Remove the window
	DestroyWindow(hwnd);
	hwnd = nullptr;

	//Remove the application instance by un-registering our window class
	UnregisterClass(appName, hInstance);
	hInstance = nullptr;

	//Release the pointer to this object
	applicationHandle = nullptr;
}

//This is where windows will sends its message to for us to intercept and use with our message handler, if we can't use it then we just return it back to the main windows message handler inside the MessageHandler function
//We intercept this by giving this prototype function to the window procedure when we defined the window class structure for our window class (WNDCLASSEX), this way we hook into the messaging functionality and intercept messages
LRESULT CALLBACK WndProc(HWND const hwnd, UINT const umsg, WPARAM const wparam, LPARAM const lparam) {
	
	switch(umsg)
	{
		//Check if the window is being destroyed
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		//Check if the window is being closed
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		//All other messages pass to our message handler
		default:
		{
			return applicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
		}
	}
}

void GraphicsEngine::Shutdown() {
	try
	{

	}
	catch (exception& e)
	{
		ShutdownWindows();
	}
}