#include "GraphicsRenderer.h"

GraphicsRenderer::GraphicsRenderer(int screenWidth, int screenHeight, HWND const hwnd) :  initializationFailed(false),  d3D(nullptr), antTweakBarStatistics(nullptr), camera(nullptr), lightManager(nullptr),  terrain(nullptr),  rocket(nullptr),  displacedFloor(nullptr),   skyBox(nullptr),   gameObjects(),  shaderManager(nullptr),  resourceManager(nullptr),   shadowMapManager(nullptr),  renderToggle(0),  renderOptionalGameObjects(false),  timeScale(1),  updateCamera(false),  cameraMode(0),  dt(0.0f),  fps(0.0f),  start({0}),  end({0}),  frequency({0}) {
	//Create D3D object
	 d3D = make_shared<D3DContainer>(screenWidth, screenHeight, hwnd, FULL_SCREEN, VSYNC_ENABLED, SCREEN_DEPTH, SCREEN_NEAR);

	 windowWidth = screenWidth;
	 windowHeight = screenHeight;


	// Initialize AntTweakBar
	if (!TwInit(TW_DIRECT3D11,  d3D->GetDevice()))
	{
		 initializationFailed = true;
		MessageBox(hwnd, "AntTweak failed to Initialize", "Error", MB_OK);
		return;
	}

	TwWindowSize(screenWidth, screenHeight);

	 antTweakBarStatistics = TwNewBar("Statistics");
	TwDefine(" Statistics label='Statistics' position='20 20' size='240 160' alpha=0");
	TwAddVarRW( antTweakBarStatistics, "WindowWidth: ", TW_TYPE_FLOAT, & windowWidth, "");
	TwAddVarRW( antTweakBarStatistics, "WindowHeight: ", TW_TYPE_FLOAT, & windowHeight, "");
	TwAddVarRW( antTweakBarStatistics, "DT: ", TW_TYPE_FLOAT, & dt, "");
	TwAddVarRW( antTweakBarStatistics, "FPS: ", TW_TYPE_FLOAT, & fps, "");
	TwAddVarRW( antTweakBarStatistics, "Render Mode: ", TW_TYPE_INT32, & renderToggle, "");
	TwAddVarRW( antTweakBarStatistics, "Camera Mode: ", TW_TYPE_INT32, & cameraMode, "");
	TwAddVarRW( antTweakBarStatistics, "TimeScale: ", TW_TYPE_INT32, & timeScale, "");
	TwAddVarRW( antTweakBarStatistics, "Optional GameObjects: ", TW_TYPE_BOOLCPP, & renderOptionalGameObjects, "");

	 configuration = make_shared<SimulationConfigLoader>("Configuration.txt");

	if ( d3D->GetInitializationState()) {
		 initializationFailed = true;
		MessageBox(hwnd, "Direct3D failed to Initialize", "Error", MB_OK);
		return;
	}

	 shaderManager = make_shared<ShaderManager>( d3D->GetDevice(), hwnd);

	if ( shaderManager->GetInitializationState())
	{
		 initializationFailed = true;
		MessageBox(hwnd, "ShaderManager failed to Initialize", "Error", MB_OK);
		return;
	}

	 resourceManager = make_shared<ResourceManager>();

	if (! resourceManager)
	{
		 initializationFailed = true;
		MessageBox(hwnd, "ResourceManager failed to Initialize", "Error", MB_OK);
		return;
	}

	 particleSystemManager = make_shared<ParticleSystemManager>();

	//Create camera
	 camera = make_shared<Camera>();

	if (! camera)
	{
		 initializationFailed = true;
		MessageBox(hwnd, "Camera failed to Initialize", "Error", MB_OK);
		return;
	}

	//Set camera position
	 camera->SetPosition( configuration->GetCameraPosition());

	 lightManager = make_shared<LightManager>();

	if (! lightManager)
	{
		 initializationFailed = true;
		MessageBox(hwnd, "LightManager failed to Initialize", "Error", MB_OK);
		return;
	}

	const auto terrainDimensions =  configuration->GetTerrainDimensions();

	auto rocketPosition =  configuration->GetRocketPosition();
	rocketPosition.x += -terrainDimensions.z;

	 terrain = make_shared<Terrain>( d3D->GetDevice(), XMFLOAT3(80, 10, 40), XMFLOAT3(1, 1, 1),  shaderManager->GetTextureDisplacementShader(),  resourceManager);
	 rocket = make_shared<Rocket>( d3D->GetDevice(), rocketPosition,  configuration->GetRocketRotation(),  configuration->GetRocketScale(),  shaderManager,  resourceManager);

	 lightManager->AddLight(XMFLOAT3(0.0f, 0.0f, -terrainDimensions.z), XMFLOAT3(0.0f, 0.0f, 0.0f),  configuration->GetSunAmbient(),  configuration->GetSunDiffuse(),  configuration->GetSunSpecular(),  configuration->GetSunSpecularPower(), terrainDimensions.x, terrainDimensions.z, 1, terrainDimensions.z, true, true);
	 lightManager->AddLight(XMFLOAT3(-terrainDimensions.x, -terrainDimensions.x, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),  configuration->GetSunAmbient(),  configuration->GetMoonDiffuse(),  configuration->GetMoonSpecular(),  configuration->GetMoonSpecularPower(), terrainDimensions.z, terrainDimensions.x, 1, terrainDimensions.x, true, true);

	 shadowMapManager = make_shared<ShadowMapManager>(hwnd,  d3D->GetDevice(),  shaderManager->GetDepthShader(),  lightManager->GetLightList().size(), SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

	vector<const WCHAR*> textureNames;
	textureNames.push_back(L"BaseColour.dds");
	textureNames.push_back(L"BaseNormal.dds");
	textureNames.push_back(L"BaseSpecular.dds");
	textureNames.push_back(L"BaseDisplacement.dds");

	 gameObjects.push_back(make_shared<GameObject>());
	 gameObjects.back()->AddPositionComponent(XMFLOAT3(8.0f, 0.0f, 0.0f));
	 gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	 gameObjects.back()->AddScaleComponent(1.0f, 1.0f, 1.0f);
	 gameObjects.back()->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	 gameObjects.back()->AddModelComponent( d3D->GetDevice(), ModelType::Sphere,  resourceManager);
	 gameObjects.back()->AddTextureComponent( d3D->GetDevice(), textureNames,  resourceManager);
	 gameObjects.back()->SetShaderComponent( shaderManager->GetTextureDisplacementShader());
	 gameObjects.back()->SetDisplacementVariables(20.0f, 0.0f, 6.0f, 0.15f);

	if ( gameObjects.back()->GetInitializationState())
	{
		 initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	/*textureNames.push_back(L"BodyColour.dds");
	textureNames.push_back(L"BodyNormal.dds");
	textureNames.push_back(L"BodySpecular.dds");
	textureNames.push_back(L"BodyDisplacement.dds");*/

	textureNames.push_back(L"BaseColour.dds");
	textureNames.push_back(L"BaseNormal.dds");
	textureNames.push_back(L"BaseSpecular.dds");
	textureNames.push_back(L"BaseDisplacement.dds");

	 gameObjects.push_back(make_shared<GameObject>());
	 gameObjects.back()->AddPositionComponent(XMFLOAT3(5.0f, 2.0f, 0.0f));
	 gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	 gameObjects.back()->AddScaleComponent(1.0f, 6.0f, 1.0f);
	 gameObjects.back()->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	 gameObjects.back()->AddModelComponent( d3D->GetDevice(), ModelType::HighPolyCylinder,  resourceManager);
	 gameObjects.back()->AddTextureComponent( d3D->GetDevice(), textureNames,  resourceManager);
	 gameObjects.back()->SetShaderComponent( shaderManager->GetTextureDisplacementShader());
	 gameObjects.back()->SetTessellationVariables(5.0f, 20.0f, 8.0f, 1.0f);
	 gameObjects.back()->SetDisplacementVariables(20.0f, 0.0f, 6.0f, 0.15f);

	if ( gameObjects.back()->GetInitializationState())
	{
		 initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	textureNames.push_back(L"BaseColour.dds");
	textureNames.push_back(L"BaseNormal.dds");
	textureNames.push_back(L"BaseSpecular.dds");
	textureNames.push_back(L"BaseDisplacement.dds");

	const auto launchPadTessellationValues =  configuration->GetLaunchPadTessellationValues();
	const auto launchPadDisplacementValues =  configuration->GetLaunchPadDisplacementValues();

	//Launch pad
	 displacedFloor = make_shared<GameObject>();
	 displacedFloor->AddScaleComponent( configuration->GetLaunchPadScale());
	 displacedFloor->AddPositionComponent(XMFLOAT3(-terrainDimensions.z, 0.5f, 0.0f));
	 displacedFloor->AddRotationComponent(0.0f, 0.0f, 0.0f);
	 displacedFloor->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	 displacedFloor->AddModelComponent( d3D->GetDevice(), ModelType::Plane,  resourceManager);
	 displacedFloor->AddTextureComponent( d3D->GetDevice(), textureNames,  resourceManager);
	 displacedFloor->SetShaderComponent( shaderManager->GetTextureDisplacementShader());
	 displacedFloor->SetTessellationVariables(launchPadTessellationValues.x, launchPadTessellationValues.y, launchPadTessellationValues.z, launchPadTessellationValues.w);
	 displacedFloor->SetDisplacementVariables(launchPadDisplacementValues.x, launchPadDisplacementValues.y, launchPadDisplacementValues.z, launchPadDisplacementValues.w);

	if ( gameObjects.back()->GetInitializationState())
	{
		 initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	textureNames.push_back(L"FloorColour.dds");
	textureNames.push_back(L"FloorNormal.dds");
	textureNames.push_back(L"FloorSpecular.dds");
	textureNames.push_back(L"FloorDisplacement.dds");

	 gameObjects.push_back(make_shared<GameObject>());
	 gameObjects.back()->AddScaleComponent(1.0f, 1.0f, 1.0f);
	 gameObjects.back()->AddPositionComponent(XMFLOAT3(-2.0f, 0.0f, 0.0f));
	 gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	 gameObjects.back()->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	 gameObjects.back()->AddModelComponent( d3D->GetDevice(), ModelType::HighPolyCube,  resourceManager);
	 gameObjects.back()->AddTextureComponent( d3D->GetDevice(), textureNames,  resourceManager);
	 gameObjects.back()->SetShaderComponent( shaderManager->GetTextureDisplacementShader());
	 gameObjects.back()->SetDisplacementVariables(5.0f, 0.0f, 6.0f, 0.02f);

	if ( gameObjects.back()->GetInitializationState())
	{
		 initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	textureNames.push_back(L"skybox.dds");

	 skyBox = make_shared<GameObject>();
	 skyBox->AddPositionComponent(0.0f, 0.0f, 0.0f);
	 skyBox->AddRotationComponent(0.0f, 0.0f, 0.0f);
	 skyBox->AddScaleComponent(300.0f, 300.0f, 300.0f);
	 skyBox->AddModelComponent( d3D->GetDevice(), ModelType::SphereInverted,  resourceManager);
	 skyBox->AddTextureComponent( d3D->GetDevice(), textureNames,  resourceManager);
	 skyBox->SetShaderComponent( shaderManager->GetTextureCubeShader());

	 gameObjects.push_back(make_shared<GameObject>());
	 gameObjects.back()->AddPositionComponent(0.0f, 0.0f, 0.0f);
	 gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	 gameObjects.back()->AddScaleComponent(2.0f, 2.0f, 2.0f);
	 gameObjects.back()->AddModelComponent( d3D->GetDevice(), ModelType::Sphere,  resourceManager);
	 gameObjects.back()->AddTextureComponent( d3D->GetDevice(), textureNames,  resourceManager);
	 gameObjects.back()->SetShaderComponent( shaderManager->GetReflectionShader());

	QueryPerformanceFrequency(& frequency);
	QueryPerformanceCounter(& start);
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
	return  camera;
}

void GraphicsRenderer::ToggleRenderOption()
{
	 renderToggle++;

	if ( renderToggle == 5)
	{
		 renderToggle = 0;
	}

	switch ( renderToggle)
	{
		case 0:
			 shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(0, 0, 0);
			break;
		case 1:
			 d3D->EnableWireFrame();
			break;
		case 2:
			 shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(1, 0, 1);
			 d3D->DisableWireFrame();
			break;
		case 3:
			 shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(0, 1, 1);
			break;
		case 4:
			 shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(0, 1, 0);
			break;
		default: return;
	}
}

void GraphicsRenderer::ToggleOptionalGameObjects()
{
	 renderOptionalGameObjects = ! renderOptionalGameObjects;
}


void GraphicsRenderer::ResetToInitialState() const
{
	 rocket->ResetRocketState();
	 terrain->ResetTerrainState();
	 particleSystemManager->ResetParticleSystems();
}

void GraphicsRenderer::AddTimeScale(const int number)
{
	 timeScale += number;

	if ( timeScale < 1)
	{
		 timeScale = 1;
	}
}

void GraphicsRenderer::RotateRocketLeft() const
{
	 rocket->AdjustRotationLeft();
}

void GraphicsRenderer::RotateRocketRight() const
{
	 rocket->AdjustRotationRight();
}

void GraphicsRenderer::LaunchRocket() const
{
	 rocket->LaunchRocket();
}

void GraphicsRenderer::ChangeCameraMode(const int camMode)
{
	 cameraMode = camMode;

	switch ( cameraMode)
	{
	case 0:
		const auto position =  rocket->GetLauncherPosition();
		 camera->SetPosition(position.x, position.y, position.z - 10.0f);
		 camera->SetRotation(0.0f, 0.0f, 0.0f);
		 updateCamera = false;
		break;
	case 1:
		 camera->SetPosition(0.0f, 10.0f, -30.0f);
		 camera->SetRotation(0.0f, 45.0f, 0.0f);
		 updateCamera = false;
		break;
	case 2:
		 camera->SetRotation(0.0f, 0.0f, 0.0f);
		 updateCamera = true;
		break;
	case 3:
		 camera->SetRotation(0.0f, 0.0f, 0.0f);
		 updateCamera = true;
		break;
	case 4:
		 camera->SetRotation(0.0f, 0.0f, 0.0f);
		 updateCamera = true;
		break;
	default:
		break;
	}
}

void GraphicsRenderer::UpdateCameraPosition() const
{
	if ( updateCamera)
	{
		switch ( cameraMode)
		{
		case 2:
			const auto rocketPosition =  rocket->GetLookAtRocketPosition();
			 camera->SetPosition(rocketPosition.x, rocketPosition.y, rocketPosition.z - 20.0f);
			break;
		case 3:
			const auto rocketConePosition =  rocket->GetLookAtRocketConePosition();
			 camera->SetPosition(rocketConePosition.x, rocketConePosition.y, rocketConePosition.z - 3.0f);
			break;
		case 4:
			const auto rocketBodyPosition =  rocket->GetLookAtRocketPosition();
			 camera->SetPosition(rocketBodyPosition.x, rocketBodyPosition.y, rocketBodyPosition.z - 3.0f);
			break;
		default:
			break;
		}
	}
}


bool GraphicsRenderer::UpdateFrame() {

	//Calculate delta
	QueryPerformanceCounter(& end);
	 dt = static_cast<float>(( end.QuadPart -  start.QuadPart) / static_cast<double>( frequency.QuadPart));
	 start =  end;

	//Apply timescale
	 dt *=  timeScale;

	//Calculate fps
	 fps = static_cast<int>(1.0 /  dt);

	 displacedFloor->Update();

	 skyBox->Update();

	for (const auto& gameObject :  gameObjects)
	{
		gameObject->Update();
	}

	auto collisionPosition = XMFLOAT3();
	auto blastRadius = 0.0f;

	const auto rocketCollisionState =  rocket->CheckForTerrainCollision( terrain, collisionPosition, blastRadius);

	if (rocketCollisionState)
	{
		 particleSystemManager->GenerateExplosion( d3D->GetDevice(), collisionPosition, blastRadius,  resourceManager);
	}

	 terrain->UpdateTerrain();
	 rocket->UpdateRocket( dt);

	UpdateCameraPosition();

	for (const auto& light :  lightManager->GetLightList())
	{
		light->UpdateLightVariables( dt);
	}

	 particleSystemManager->Update( dt);

	//Render the graphics scene
	auto const result = RenderFrame();

	return result;
}

bool GraphicsRenderer::RenderFrame() {

	auto result = true;

	//Clear the buffer and render the scene

	 camera->Render();

	vector<shared_ptr<GameObject>> gameObjects;

	if ( renderOptionalGameObjects)
	{
		for (auto gameObject :  gameObjects)
		{
			gameObjects.emplace_back(gameObject);
		}
	}

	//gameObjects.emplace_back(make_shared<GameObject>(* terrain));
	gameObjects.emplace_back(static_pointer_cast<GameObject>( terrain));
	gameObjects.emplace_back( displacedFloor);
	gameObjects.emplace_back( skyBox);
	gameObjects.emplace_back( rocket->GetRocketBody());
	gameObjects.emplace_back( rocket->GetRocketCone());
	gameObjects.emplace_back( rocket->GetRocketCap());
	gameObjects.emplace_back( rocket->GetRocketLauncher());

	//gameObjects.emplace_back(static_cast<shared_ptr<Terrain>>( terrain));

	//Generate shadow maps
	 shadowMapManager->GenerateShadowMapResources( d3D->GetDeviceContext(),  d3D->GetDepthStencilView(),  lightManager->GetLightList(), gameObjects,  camera->GetPosition());

	 d3D->SetRenderTarget();

	XMMATRIX viewMatrix = {};
	XMMATRIX projectionMatrix = {};

	//Clear the buffer and render the scene
	 d3D->BeginScene(1.0f, 0.0f, 0.0f, 1.0f);

	 camera->GetViewMatrix(viewMatrix);
	 d3D->GetProjectionMatrix(projectionMatrix);

	vector<shared_ptr<Light>> lightList =  lightManager->GetLightList();

	if ( rocket->RocketLaunched() &&  rocket->ParticleSystemActive())
	{
		lightList.emplace_back( rocket->GetParticleSystemLight());
	}

	const auto explosionLights =  particleSystemManager->GetLights();

	if (!explosionLights.empty())
	{
		for (auto& light : explosionLights)
		{
			lightList.emplace_back(light);
		}
	}

	result =  displacedFloor->Render( d3D->GetDeviceContext(), viewMatrix, projectionMatrix,  shadowMapManager->GetShadowMapResources(), lightList,  camera->GetPosition());

	if (!result)
	{
		return false;
	}

	result =  skyBox->Render( d3D->GetDeviceContext(), viewMatrix, projectionMatrix,  shadowMapManager->GetShadowMapResources(), lightList,  camera->GetPosition());

	if (!result)
	{
		return false;
	}

	if ( renderOptionalGameObjects)
	{
		for (const auto& gameObject :  gameObjects)
		{
			result = gameObject->Render( d3D->GetDeviceContext(), viewMatrix, projectionMatrix,  shadowMapManager->GetShadowMapResources(), lightList,  camera->GetPosition());
			//result = gameObject->Render( d3D->GetDeviceContext(),  lightManager->GetPointLightList()[0]->GetLightViewMatrix(), projectionMatrix,  shadowMapManager->GetShadowMapResources(),  lightManager->GetPointLightList(),  camera->GetPosition());

			if (!result)
			{
				return false;
			}
		}
	}

	result =  terrain->RenderTerrain( d3D->GetDeviceContext(), viewMatrix, projectionMatrix,  shadowMapManager->GetShadowMapResources(), lightList,  camera->GetPosition());
	result =  rocket->RenderRocket( d3D, viewMatrix, projectionMatrix,  shadowMapManager->GetShadowMapResources(), lightList,  camera->GetPosition());

	 d3D->DisableDepthStencil();
	 d3D->EnableAlphaBlending();

	result =  particleSystemManager->Render( d3D->GetDeviceContext(), viewMatrix, projectionMatrix,  camera->GetPosition());

	if (!result)
	{
		return false;
	}

	 d3D->EnabledDepthStencil();
	 d3D->DisableAlphaBlending();

	TwDraw();

	//Present the scene
	 d3D->EndScene();

	return true;
}

bool GraphicsRenderer::GetInitializationState() const {
	return  initializationFailed;
}
