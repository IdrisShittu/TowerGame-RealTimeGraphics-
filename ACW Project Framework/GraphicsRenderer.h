#pragma once

#include <Windows.h>

#include "D3DContainer.h"

#include "Camera.h"
#include "GameObject.h"
#include "ShaderManager.h"
#include "ResourceManager.h"
#include "LightManager.h"
#include "RenderToTexture.h"
#include "ShadowMapManager.h"
#include "ParticleSystem.h"
#include "FireJetParticleSystem.h"
#include "SmokeParticleSystem.h"
#include "Terrain.h"
#include "Rocket.h"
#include "ParticleSystemManager.h"
#include "AntTweakBar.h"
#include "ConfigurationFileLoader.h"

auto const FULL_SCREEN = false;
auto const VSYNC_ENABLED = true;
auto const SCREEN_DEPTH = 1000.0f;
auto const SCREEN_NEAR = 0.1f;

const int SHADOW_MAP_WIDTH = 1360;
const int SHADOW_MAP_HEIGHT = 720;

class GraphicsRenderer
{
public:
	GraphicsRenderer(int screenWidth, int screenHeight, HWND const hwnd); // Default Constructor
	GraphicsRenderer(const GraphicsRenderer& other); // Copy Constructor
	GraphicsRenderer(GraphicsRenderer&& other) noexcept; // Move Constructor
	~GraphicsRenderer(); // Destructor

	GraphicsRenderer& operator = (const GraphicsRenderer& other); // Copy Assignment Operator
	GraphicsRenderer& operator = (GraphicsRenderer&& other) noexcept; // Move Assignment Operator

	const shared_ptr<Camera>& GetCamera() const;

	void ToggleRenderOption();
	void ToggleOptionalGameObjects();
	void ResetToInitialState() const;
	void AddTimeScale(const int number);
	void RotateRocketLeft() const;
	void RotateRocketRight() const;
	void LaunchRocket() const;
	void ChangeCameraMode(const int cameraMode);
	void UpdateCameraPosition() const;

	bool UpdateFrame();

	bool GetInitializationState() const;

private:
	bool RenderFrame();

	bool m_initializationFailed;

	shared_ptr<ConfigurationFileLoader> m_configuration;

	shared_ptr<D3DContainer> m_d3D;

	TwBar* m_antTweakBarStatistics;

	shared_ptr<Camera> m_camera;
	shared_ptr<LightManager> m_lightManager;

	shared_ptr<Terrain> m_terrain;
	shared_ptr<Rocket> m_rocket;

	shared_ptr<GameObject> m_displacedFloor;
	shared_ptr<GameObject> m_skyBox;

	vector<shared_ptr<GameObject>> m_gameObjects;

	shared_ptr<ShaderManager> m_shaderManager;
	shared_ptr<ResourceManager> m_resourceManager;
	shared_ptr<ParticleSystemManager> m_particleSystemManager;
	shared_ptr<ShadowMapManager> m_shadowMapManager;

	float m_windowWidth;
	float m_windowHeight;

	int m_renderToggle;
	bool m_renderOptionalGameObjects;
	int m_timeScale;
	bool m_updateCamera;
	int m_cameraMode;

	float m_dt;
	float m_fps;
	LARGE_INTEGER m_start;
	LARGE_INTEGER m_end;
	LARGE_INTEGER m_frequency;
};