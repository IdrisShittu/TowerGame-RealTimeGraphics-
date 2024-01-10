#include "GraphicsRenderer.h"

GraphicsRenderer::GraphicsRenderer(int screenWidth, int screenHeight, HWND const hwnd) : m_initializationFailed(false), m_d3D(nullptr), m_antTweakBarStatistics(nullptr), m_camera(nullptr), m_lightManager(nullptr), m_terrain(nullptr), m_rocket(nullptr), m_displacedFloor(nullptr), m_skyBox(nullptr), m_gameObjects(), m_shaderManager(nullptr), m_resourceManager(nullptr), m_shadowMapManager(nullptr), m_renderToggle(0), m_renderOptionalGameObjects(false), m_timeScale(1), m_updateCamera(false), m_cameraMode(0), m_dt(0.0f), m_fps(0.0f), m_start({0}), m_end({0}), m_frequency({0}) {
	//Create D3D object
	m_d3D = make_shared<D3DContainer>(screenWidth, screenHeight, hwnd, FULL_SCREEN, VSYNC_ENABLED, SCREEN_DEPTH, SCREEN_NEAR);

	m_windowWidth = screenWidth;
	m_windowHeight = screenHeight;

	// Initialize AntTweakBar
	if (!TwInit(TW_DIRECT3D11, m_d3D->GetDevice()))
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "AntTweak failed to Initialize", "Error", MB_OK);
		return;
	}

	TwWindowSize(screenWidth, screenHeight);

	m_antTweakBarStatistics = TwNewBar("Statistics");
	TwDefine(" Statistics label='Statistics' position='20 20' size='240 160' alpha=0");
	TwAddVarRW(m_antTweakBarStatistics, "WindowWidth: ", TW_TYPE_FLOAT, &m_windowWidth, "");
	TwAddVarRW(m_antTweakBarStatistics, "WindowHeight: ", TW_TYPE_FLOAT, &m_windowHeight, "");
	TwAddVarRW(m_antTweakBarStatistics, "DT: ", TW_TYPE_FLOAT, &m_dt, "");
	TwAddVarRW(m_antTweakBarStatistics, "FPS: ", TW_TYPE_FLOAT, &m_fps, "");
	TwAddVarRW(m_antTweakBarStatistics, "Render Mode: ", TW_TYPE_INT32, &m_renderToggle, "");
	TwAddVarRW(m_antTweakBarStatistics, "Camera Mode: ", TW_TYPE_INT32, &m_cameraMode, "");
	TwAddVarRW(m_antTweakBarStatistics, "TimeScale: ", TW_TYPE_INT32, &m_timeScale, "");
	TwAddVarRW(m_antTweakBarStatistics, "Optional GameObjects: ", TW_TYPE_BOOLCPP, &m_renderOptionalGameObjects, "");

	m_configuration = make_shared<ConfigurationFileLoader>("Configuration.txt");

	if (m_d3D->GetInitializationState()) {
		m_initializationFailed = true;
		MessageBox(hwnd, "Direct3D failed to Initialize", "Error", MB_OK);
		return;
	}

	m_shaderManager = make_shared<ShaderManager>(m_d3D->GetDevice(), hwnd);

	if (m_shaderManager->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "ShaderManager failed to Initialize", "Error", MB_OK);
		return;
	}

	m_resourceManager = make_shared<ResourceManager>();

	if (!m_resourceManager)
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "ResourceManager failed to Initialize", "Error", MB_OK);
		return;
	}

	m_particleSystemManager = make_shared<ParticleSystemManager>();

	//Create camera
	m_camera = make_shared<Camera>();

	if (!m_camera)
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "Camera failed to Initialize", "Error", MB_OK);
		return;
	}

	//Set camera position
	m_camera->SetPosition(m_configuration->GetCameraPosition());

	m_lightManager = make_shared<LightManager>();

	if (!m_lightManager)
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "LightManager failed to Initialize", "Error", MB_OK);
		return;
	}

	const auto terrainDimensions = m_configuration->GetTerrainDimensions();

	auto rocketPosition = m_configuration->GetRocketPosition();
	rocketPosition.x += -terrainDimensions.z;

	m_terrain = make_shared<Terrain>(m_d3D->GetDevice(), XMFLOAT3(80, 10, 40), XMFLOAT3(1, 1, 1), m_shaderManager->GetTextureDisplacementShader(), m_resourceManager);
	m_rocket = make_shared<Rocket>(m_d3D->GetDevice(), rocketPosition, m_configuration->GetRocketRotation(), m_configuration->GetRocketScale(), m_shaderManager, m_resourceManager);

	m_lightManager->AddLight(XMFLOAT3(0.0f, 0.0f, -terrainDimensions.z), XMFLOAT3(0.0f, 0.0f, 0.0f), m_configuration->GetSunAmbient(), m_configuration->GetSunDiffuse(), m_configuration->GetSunSpecular(), m_configuration->GetSunSpecularPower(), terrainDimensions.x, terrainDimensions.z, 1, terrainDimensions.z, true, true);
	m_lightManager->AddLight(XMFLOAT3(-terrainDimensions.x, -terrainDimensions.x, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), m_configuration->GetSunAmbient(), m_configuration->GetMoonDiffuse(), m_configuration->GetMoonSpecular(), m_configuration->GetMoonSpecularPower(), terrainDimensions.z, terrainDimensions.x, 1, terrainDimensions.x, true, true);

	m_shadowMapManager = make_shared<ShadowMapManager>(hwnd, m_d3D->GetDevice(), m_shaderManager->GetDepthShader(), m_lightManager->GetLightList().size(), SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

	vector<const WCHAR*> textureNames;
	textureNames.push_back(L"IceColour.dds");
	textureNames.push_back(L"IceNormal.dds");
	textureNames.push_back(L"IceSpecular.dds");
	textureNames.push_back(L"IceDisplacement.dds");

	m_gameObjects.push_back(make_shared<GameObject>());
	m_gameObjects.back()->AddPositionComponent(XMFLOAT3(8.0f, 0.0f, 0.0f));
	m_gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_gameObjects.back()->AddScaleComponent(1.0f, 1.0f, 1.0f);
	m_gameObjects.back()->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	m_gameObjects.back()->AddModelComponent(m_d3D->GetDevice(), ModelType::Sphere, m_resourceManager);
	m_gameObjects.back()->AddTextureComponent(m_d3D->GetDevice(), textureNames, m_resourceManager);
	m_gameObjects.back()->SetShaderComponent(m_shaderManager->GetTextureDisplacementShader());
	m_gameObjects.back()->SetDisplacementVariables(20.0f, 0.0f, 6.0f, 0.15f);

	if (m_gameObjects.back()->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	textureNames.push_back(L"MetalCrystalColour.dds");
	textureNames.push_back(L"MetalCrystalNormal.dds");
	textureNames.push_back(L"MetalCrystalSpecular.dds");
	textureNames.push_back(L"MetalCrystalDisplacement.dds");

	m_gameObjects.push_back(make_shared<GameObject>());
	m_gameObjects.back()->AddPositionComponent(XMFLOAT3(5.0f, 2.0f, 0.0f));
	m_gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_gameObjects.back()->AddScaleComponent(1.0f, 6.0f, 1.0f);
	m_gameObjects.back()->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	m_gameObjects.back()->AddModelComponent(m_d3D->GetDevice(), ModelType::HighPolyCylinder, m_resourceManager);
	m_gameObjects.back()->AddTextureComponent(m_d3D->GetDevice(), textureNames, m_resourceManager);
	m_gameObjects.back()->SetShaderComponent(m_shaderManager->GetTextureDisplacementShader());
	m_gameObjects.back()->SetTessellationVariables(5.0f, 20.0f, 8.0f, 1.0f);
	m_gameObjects.back()->SetDisplacementVariables(20.0f, 0.0f, 6.0f, 0.15f);

	if (m_gameObjects.back()->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	textureNames.push_back(L"IceColour.dds");
	textureNames.push_back(L"IceNormal.dds");
	textureNames.push_back(L"IceSpecular.dds");
	textureNames.push_back(L"IceDisplacement.dds");

	const auto launchPadTessellationValues = m_configuration->GetLaunchPadTessellationValues();
	const auto launchPadDisplacementValues = m_configuration->GetLaunchPadDisplacementValues();

	//Launch pad
	m_displacedFloor = make_shared<GameObject>();
	m_displacedFloor->AddScaleComponent(m_configuration->GetLaunchPadScale());
	m_displacedFloor->AddPositionComponent(XMFLOAT3(-terrainDimensions.z, 0.5f, 0.0f));
	m_displacedFloor->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_displacedFloor->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	m_displacedFloor->AddModelComponent(m_d3D->GetDevice(), ModelType::Plane, m_resourceManager);
	m_displacedFloor->AddTextureComponent(m_d3D->GetDevice(), textureNames, m_resourceManager);
	m_displacedFloor->SetShaderComponent(m_shaderManager->GetTextureDisplacementShader());
	m_displacedFloor->SetTessellationVariables(launchPadTessellationValues.x, launchPadTessellationValues.y, launchPadTessellationValues.z, launchPadTessellationValues.w);
	m_displacedFloor->SetDisplacementVariables(launchPadDisplacementValues.x, launchPadDisplacementValues.y, launchPadDisplacementValues.z, launchPadDisplacementValues.w);

	if (m_gameObjects.back()->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	textureNames.push_back(L"StoneFloorColour.dds");
	textureNames.push_back(L"StoneFloorNormal.dds");
	textureNames.push_back(L"StoneFloorSpecular.dds");
	textureNames.push_back(L"StoneFloorDisplacement.dds");

	m_gameObjects.push_back(make_shared<GameObject>());
	m_gameObjects.back()->AddScaleComponent(1.0f, 1.0f, 1.0f);
	m_gameObjects.back()->AddPositionComponent(XMFLOAT3(-2.0f, 0.0f, 0.0f));
	m_gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_gameObjects.back()->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	m_gameObjects.back()->AddModelComponent(m_d3D->GetDevice(), ModelType::HighPolyCube, m_resourceManager);
	m_gameObjects.back()->AddTextureComponent(m_d3D->GetDevice(), textureNames, m_resourceManager);
	m_gameObjects.back()->SetShaderComponent(m_shaderManager->GetTextureDisplacementShader());
	m_gameObjects.back()->SetDisplacementVariables(5.0f, 0.0f, 6.0f, 0.02f);

	if (m_gameObjects.back()->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	textureNames.push_back(L"Mars.dds");

	m_skyBox = make_shared<GameObject>();
	m_skyBox->AddPositionComponent(0.0f, 0.0f, 0.0f);
	m_skyBox->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_skyBox->AddScaleComponent(300.0f, 300.0f, 300.0f);
	m_skyBox->AddModelComponent(m_d3D->GetDevice(), ModelType::SphereInverted, m_resourceManager);
	m_skyBox->AddTextureComponent(m_d3D->GetDevice(), textureNames, m_resourceManager);
	m_skyBox->SetShaderComponent(m_shaderManager->GetTextureCubeShader());

	m_gameObjects.push_back(make_shared<GameObject>());
	m_gameObjects.back()->AddPositionComponent(0.0f, 0.0f, 0.0f);
	m_gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_gameObjects.back()->AddScaleComponent(2.0f, 2.0f, 2.0f);
	m_gameObjects.back()->AddModelComponent(m_d3D->GetDevice(), ModelType::Sphere, m_resourceManager);
	m_gameObjects.back()->AddTextureComponent(m_d3D->GetDevice(), textureNames, m_resourceManager);
	m_gameObjects.back()->SetShaderComponent(m_shaderManager->GetReflectionShader());

	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_start);
}

GraphicsRenderer::GraphicsRenderer(const GraphicsRenderer& other) = default;

GraphicsRenderer::GraphicsRenderer(GraphicsRenderer&& other) noexcept = default;

GraphicsRenderer::~GraphicsRenderer()
{
	TwTerminate();
}

GraphicsRenderer& GraphicsRenderer::operator=(const GraphicsRenderer&) = default;

GraphicsRenderer& GraphicsRenderer::operator=(GraphicsRenderer&&) noexcept = default;

const shared_ptr<Camera>& GraphicsRenderer::GetCamera() const
{
	return m_camera;
}

void GraphicsRenderer::ToggleRenderOption()
{
	m_renderToggle++;

	if (m_renderToggle == 5)
	{
		m_renderToggle = 0;
	}

	switch (m_renderToggle)
	{
		case 0:
			m_shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(0, 0, 0);
			break;
		case 1:
			m_d3D->EnableWireFrame();
			break;
		case 2:
			m_shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(1, 0, 1);
			m_d3D->DisableWireFrame();
			break;
		case 3:
			m_shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(0, 1, 1);
			break;
		case 4:
			m_shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(0, 1, 0);
			break;
		default: return;
	}
}

void GraphicsRenderer::ToggleOptionalGameObjects()
{
	m_renderOptionalGameObjects = !m_renderOptionalGameObjects;
}


void GraphicsRenderer::ResetToInitialState() const
{
	m_rocket->ResetRocketState();
	m_terrain->ResetTerrainState();
	m_particleSystemManager->ResetParticleSystems();
}

void GraphicsRenderer::AddTimeScale(const int number)
{
	m_timeScale += number;

	if (m_timeScale < 1)
	{
		m_timeScale = 1;
	}
}

void GraphicsRenderer::RotateRocketLeft() const
{
	m_rocket->AdjustRotationLeft();
}

void GraphicsRenderer::RotateRocketRight() const
{
	m_rocket->AdjustRotationRight();
}

void GraphicsRenderer::LaunchRocket() const
{
	m_rocket->LaunchRocket();
}

void GraphicsRenderer::ChangeCameraMode(const int cameraMode)
{
	m_cameraMode = cameraMode;

	switch (m_cameraMode)
	{
	case 0:
		const auto position = m_rocket->GetLauncherPosition();
		m_camera->SetPosition(position.x, position.y, position.z - 10.0f);
		m_camera->SetRotation(0.0f, 0.0f, 0.0f);
		m_updateCamera = false;
		break;
	case 1:
		m_camera->SetPosition(0.0f, 10.0f, -30.0f);
		m_camera->SetRotation(0.0f, 45.0f, 0.0f);
		m_updateCamera = false;
		break;
	case 2:
		m_camera->SetRotation(0.0f, 0.0f, 0.0f);
		m_updateCamera = true;
		break;
	case 3:
		m_camera->SetRotation(0.0f, 0.0f, 0.0f);
		m_updateCamera = true;
		break;
	case 4:
		m_camera->SetRotation(0.0f, 0.0f, 0.0f);
		m_updateCamera = true;
		break;
	default:
		break;
	}
}

void GraphicsRenderer::UpdateCameraPosition() const
{
	if (m_updateCamera)
	{
		switch (m_cameraMode)
		{
		case 2:
			const auto rocketPosition = m_rocket->GetLookAtRocketPosition();
			m_camera->SetPosition(rocketPosition.x, rocketPosition.y, rocketPosition.z - 20.0f);
			break;
		case 3:
			const auto rocketConePosition = m_rocket->GetLookAtRocketConePosition();
			m_camera->SetPosition(rocketConePosition.x, rocketConePosition.y, rocketConePosition.z - 3.0f);
			break;
		case 4:
			const auto rocketBodyPosition = m_rocket->GetLookAtRocketPosition();
			m_camera->SetPosition(rocketBodyPosition.x, rocketBodyPosition.y, rocketBodyPosition.z - 3.0f);
			break;
		default:
			break;
		}
	}
}


bool GraphicsRenderer::UpdateFrame() {

	//Calculate delta
	QueryPerformanceCounter(&m_end);
	m_dt = static_cast<float>((m_end.QuadPart - m_start.QuadPart) / static_cast<double>(m_frequency.QuadPart));
	m_start = m_end;

	//Apply timescale
	m_dt *= m_timeScale;

	//Calculate fps
	m_fps = static_cast<int>(1.0 / m_dt);

	m_displacedFloor->Update();

	m_skyBox->Update();

	for (const auto& gameObject : m_gameObjects)
	{
		gameObject->Update();
	}

	auto collisionPosition = XMFLOAT3();
	auto blastRadius = 0.0f;

	const auto rocketCollisionState = m_rocket->CheckForTerrainCollision(m_terrain, collisionPosition, blastRadius);

	if (rocketCollisionState)
	{
		m_particleSystemManager->GenerateExplosion(m_d3D->GetDevice(), collisionPosition, blastRadius, m_resourceManager);
	}

	m_terrain->UpdateTerrain();
	m_rocket->UpdateRocket(m_dt);

	UpdateCameraPosition();

	for (const auto& light : m_lightManager->GetLightList())
	{
		light->UpdateLightVariables(m_dt);
	}

	m_particleSystemManager->Update(m_dt);

	//Render the graphics scene
	auto const result = RenderFrame();

	return result;
}

bool GraphicsRenderer::RenderFrame() {

	auto result = true;

	//Clear the buffer and render the scene

	m_camera->Render();

	vector<shared_ptr<GameObject>> gameObjects;

	if (m_renderOptionalGameObjects)
	{
		for (auto gameObject : m_gameObjects)
		{
			gameObjects.emplace_back(gameObject);
		}
	}

	//gameObjects.emplace_back(make_shared<GameObject>(*m_terrain));
	gameObjects.emplace_back(static_pointer_cast<GameObject>(m_terrain));
	gameObjects.emplace_back(m_displacedFloor);
	gameObjects.emplace_back(m_skyBox);
	gameObjects.emplace_back(m_rocket->GetRocketBody());
	gameObjects.emplace_back(m_rocket->GetRocketCone());
	gameObjects.emplace_back(m_rocket->GetRocketCap());
	gameObjects.emplace_back(m_rocket->GetRocketLauncher());

	//gameObjects.emplace_back(static_cast<shared_ptr<Terrain>>(m_terrain));

	//Generate shadow maps
	m_shadowMapManager->GenerateShadowMapResources(m_d3D->GetDeviceContext(), m_d3D->GetDepthStencilView(), m_lightManager->GetLightList(), gameObjects, m_camera->GetPosition());

	m_d3D->SetRenderTarget();

	XMMATRIX viewMatrix = {};
	XMMATRIX projectionMatrix = {};

	//Clear the buffer and render the scene
	m_d3D->BeginScene(1.0f, 0.0f, 0.0f, 1.0f);

	m_camera->GetViewMatrix(viewMatrix);
	m_d3D->GetProjectionMatrix(projectionMatrix);

	vector<shared_ptr<Light>> lightList = m_lightManager->GetLightList();

	if (m_rocket->RocketLaunched() && m_rocket->ParticleSystemActive())
	{
		lightList.emplace_back(m_rocket->GetParticleSystemLight());
	}

	const auto explosionLights = m_particleSystemManager->GetLights();

	if (!explosionLights.empty())
	{
		for (auto& light : explosionLights)
		{
			lightList.emplace_back(light);
		}
	}

	result = m_displacedFloor->Render(m_d3D->GetDeviceContext(), viewMatrix, projectionMatrix, m_shadowMapManager->GetShadowMapResources(), lightList, m_camera->GetPosition());

	if (!result)
	{
		return false;
	}

	result = m_skyBox->Render(m_d3D->GetDeviceContext(), viewMatrix, projectionMatrix, m_shadowMapManager->GetShadowMapResources(), lightList, m_camera->GetPosition());

	if (!result)
	{
		return false;
	}

	if (m_renderOptionalGameObjects)
	{
		for (const auto& gameObject : m_gameObjects)
		{
			result = gameObject->Render(m_d3D->GetDeviceContext(), viewMatrix, projectionMatrix, m_shadowMapManager->GetShadowMapResources(), lightList, m_camera->GetPosition());
			//result = gameObject->Render(m_d3D->GetDeviceContext(), m_lightManager->GetPointLightList()[0]->GetLightViewMatrix(), projectionMatrix, m_shadowMapManager->GetShadowMapResources(), m_lightManager->GetPointLightList(), m_camera->GetPosition());

			if (!result)
			{
				return false;
			}
		}
	}

	result = m_terrain->RenderTerrain(m_d3D->GetDeviceContext(), viewMatrix, projectionMatrix, m_shadowMapManager->GetShadowMapResources(), lightList, m_camera->GetPosition());
	result = m_rocket->RenderRocket(m_d3D, viewMatrix, projectionMatrix, m_shadowMapManager->GetShadowMapResources(), lightList, m_camera->GetPosition());

	m_d3D->DisableDepthStencil();
	m_d3D->EnableAlphaBlending();

	result = m_particleSystemManager->Render(m_d3D->GetDeviceContext(), viewMatrix, projectionMatrix, m_camera->GetPosition());

	if (!result)
	{
		return false;
	}

	m_d3D->EnabledDepthStencil();
	m_d3D->DisableAlphaBlending();

	TwDraw();

	//Present the scene
	m_d3D->EndScene();

	return true;
}

bool GraphicsRenderer::GetInitializationState() const {
	return m_initializationFailed;
}
