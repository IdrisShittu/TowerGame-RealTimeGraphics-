#include "Rocket.h"

Rocket::Rocket(ID3D11Device* const device, const XMFLOAT3& position, const XMFLOAT3& rotation, const XMFLOAT3& scale, const shared_ptr<ShaderManager>& shaderManager, const shared_ptr<ResourceManager>& resourceManager) : m_initializationFailed(false), m_rocketLaunched(false), m_changedParticleSystem(false), m_particleSystemActive(false), m_blastRadius(4.0f), m_initialVelocity(25.0f), m_gravity(-9.81f), m_velocity(XMFLOAT2()), m_angularVelocity(XMFLOAT2()), m_initialLauncherPosition(XMFLOAT3()), m_initialLauncherRotation(XMFLOAT3()), m_lookAtRocketPosition(XMFLOAT3()), m_lookAtRocketConePosition(XMFLOAT3()), m_rocketCone(nullptr), m_rocketBody(nullptr), m_rocketCap(nullptr), m_rocketLauncher(nullptr), m_particleSystemLight(nullptr), m_fireJetParticleSystem(nullptr), m_coneFlameParticleSystem(nullptr)
{
	m_initialLauncherPosition = position;
	m_initialLauncherRotation = rotation;

	vector<const WCHAR*> textureNames;
	
	textureNames.push_back(L"BaseColour.dds");
	textureNames.push_back(L"BaseNormal.dds");
	textureNames.push_back(L"BaseSpecular.dds");
	textureNames.push_back(L"BaseDisplacement.dds");

	m_rocketBody = make_shared<GameObject>();
	m_rocketBody->AddPositionComponent(position);
	m_rocketBody->AddRotationComponent(rotation);
	m_rocketBody->AddScaleComponent(1.0f * scale.x, 6.0f * scale.y, 1.0f * scale.z);
	m_rocketBody->AddModelComponent(device, ModelType::LowPolyCylinder, resourceManager);
	m_rocketBody->AddTextureComponent(device, textureNames, resourceManager);
	m_rocketBody->SetShaderComponent(shaderManager->GetTextureDisplacementShader());
	m_rocketBody->SetTessellationVariables(2.0f, 10.0f, 64.0f, 1.0f);
	m_rocketBody->SetDisplacementVariables(20.0f, 0.0f, 6.0f, 0.18f);

	if (m_rocketBody->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(nullptr, "Could not initialize the rocket body game object.", "Error", MB_OK);
		return;
	}

	m_rocketCap = make_shared<GameObject>();
	m_rocketCap->AddPositionComponent(0.0f, -(3.0f * scale.y), 0.0f);
	m_rocketCap->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_rocketCap->AddScaleComponent(0.92f, 0.1f, 0.92f);
	m_rocketCap->AddModelComponent(device, ModelType::Sphere, resourceManager);
	m_rocketCap->AddTextureComponent(device, textureNames, resourceManager);
	m_rocketCap->SetShaderComponent(shaderManager->GetTextureDisplacementShader());
	m_rocketCap->AddParentGameObject(m_rocketBody);

	if (m_rocketCap->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(nullptr, "Could not initialize the rocket cap game object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	textureNames.push_back(L"MetalMeshColour.dds");
	textureNames.push_back(L"MetalMeshNormal.dds");
	textureNames.push_back(L"MetalMeshSpecular.dds");
	textureNames.push_back(L"MetalMeshDisplacement.dds");

	m_rocketLauncher = make_shared<GameObject>();
	m_rocketLauncher->AddPositionComponent((1.3f * scale.x), 0.0f, 0.0f);
	m_rocketLauncher->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_rocketLauncher->AddScaleComponent(0.3f * scale.x, 8.0f * scale.y, 0.3f * scale.z);
	m_rocketLauncher->AddModelComponent(device, ModelType::LowPolyCylinder, resourceManager);
	m_rocketLauncher->AddTextureComponent(device, textureNames, resourceManager);
	m_rocketLauncher->SetShaderComponent(shaderManager->GetTextureDisplacementShader());
	m_rocketLauncher->SetTessellationVariables(2.0f, 10.0f, 64.0f, 1.0f);
	m_rocketLauncher->SetDisplacementVariables(20.0f, 0.0f, 6.0f, 0.18f);
	m_rocketLauncher->AddParentGameObject(m_rocketBody);

	if (m_rocketLauncher->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(nullptr, "Could not initialize the rocket body game object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	textureNames.push_back(L"Mars.dds");

	m_rocketCone = make_shared<GameObject>();
 	m_rocketCone->AddPositionComponent(0.0f, (3.0f * scale.y + 1.0f), 0.0f);
	m_rocketCone->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_rocketCone->AddScaleComponent(1.0f, 2.0f, 1.0f);
	m_rocketCone->AddModelComponent(device, ModelType::Cone, resourceManager);
	m_rocketCone->AddTextureComponent(device, textureNames, resourceManager);
	m_rocketCone->SetShaderComponent(shaderManager->GetReflectionShader());
	m_rocketCone->SetTessellationVariables(5.0f, 20.0f, 8.0f, 1.0f);
	m_rocketCone->AddParentGameObject(m_rocketBody);

	if (m_rocketCone->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(nullptr, "Could not initialize the rocket cone game object", "Error", MB_OK);
		return;
	}

	const auto rocketBodyScale = m_rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto rocketBodyRot = m_rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto rocketBodyPos = m_rocketBody->GetPositionComponent()->GetPositionAt(0);

	auto rocketMatrix = XMMatrixIdentity();

	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixScaling(rocketBodyScale.x, rocketBodyScale.y, rocketBodyScale.z));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rocketBodyRot.x, rocketBodyRot.y, rocketBodyRot.z)));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixTranslation(rocketBodyPos.x, rocketBodyPos.y, rocketBodyPos.z));

	auto posMatrix = XMMatrixIdentity();

	posMatrix = XMMatrixMultiply(posMatrix, XMMatrixTranslation(0.0f, -0.6f, 0.0f));

	posMatrix = posMatrix * rocketMatrix;

	auto lightScale = XMVECTOR();
	auto lightRotation = XMVECTOR();
	auto lightPosition = XMVECTOR();

	XMMatrixDecompose(&lightScale, &lightRotation, &lightPosition, posMatrix);

	posMatrix = XMMatrixIdentity();

	posMatrix = XMMatrixMultiply(posMatrix, XMMatrixTranslation(0.0f, -1.2f, 0.0f));

	posMatrix = posMatrix * rocketMatrix;

	auto lightPointPosition = XMVECTOR();
	XMMatrixDecompose(&lightScale, &lightRotation, &lightPointPosition, posMatrix);

	auto lightPositionFloat = XMFLOAT3();
	auto lightPointPositionFloat = XMFLOAT3();

	XMStoreFloat3(&lightPositionFloat, lightPosition);
	XMStoreFloat3(&lightPointPositionFloat, lightPointPosition);

	m_particleSystemLight = make_shared<Light>();
	m_particleSystemLight->SetLightOrbit(false);
	m_particleSystemLight->SetLightPosition(lightPositionFloat);
	m_particleSystemLight->SetLightPointPosition(lightPointPositionFloat);
	m_particleSystemLight->SetAmbientColour(0.15f, 0.15f, 0.15f, 1.0f);
	m_particleSystemLight->SetDiffuseColour(0.8f, 0.3f, 0.0f, 1.0f);
	m_particleSystemLight->SetSpecularColour(0.8f, 0.3f, 0.0f, 1.0f);
	m_particleSystemLight->SetSpecularPower(8.0f);
	m_particleSystemLight->GenerateLightProjectionMatrix(45.0f, 45.0f, 0.1f, 1000.0f);
	m_particleSystemLight->UpdateLightVariables(0.0f);

	m_fireJetParticleSystem = make_shared<FireJetParticleSystem>(device, nullptr, ModelType::Quad, XMFLOAT3(0.0f, -(3.0f * scale.y), 0.0f), XMFLOAT3(1.5f, 1.5f, 1.5f), XMFLOAT3(0.4f, 0.4f, 0.4f), 0.5f, 5.0f, 5.6f, 60, resourceManager);
	m_fireJetParticleSystem->AddParentGameObject(m_rocketBody);

	m_coneFlameParticleSystem = make_shared<FireJetParticleSystem>(device, nullptr, ModelType::SphereInverted, XMFLOAT3(0.0f, (4.0f * scale.y), 0.0f), XMFLOAT3(1.5f, 1.5f, 1.5f), XMFLOAT3(1.0f, 1.0f, 1.0f), 0.8f, 5.0f, 4.0f, 10, resourceManager);
	m_coneFlameParticleSystem->AddParentGameObject(m_rocketBody);

	m_rocketLauncher->Update();
}

//Rocket::Rocket(const Rocket& other) = default;

//Rocket::Rocket(Rocket&& other) noexcept = default;

Rocket::~Rocket()
{
}

//Rocket& Rocket::operator=(const Rocket& other) = default;

//Rocket& Rocket::operator=(Rocket&& other) noexcept = default;

void Rocket::AdjustRotationLeft() const
{
	if (!m_rocketLaunched)
	{
		auto launchAngle = m_rocketBody->GetRotationComponent()->GetRotationAt(0);

		launchAngle.z -= -XM_PIDIV4 / 30.0f;

		if (launchAngle.z > 0.0f)
		{
			launchAngle.z = 0.0f;
		}

		m_rocketBody->SetRotation(launchAngle);
		m_rocketLauncher->Update();
	}
}

void Rocket::AdjustRotationRight() const
{
	if (!m_rocketLaunched)
	{
		auto launchAngle = m_rocketBody->GetRotationComponent()->GetRotationAt(0);

		launchAngle.z += -XM_PIDIV4 / 30.0f;

		if (launchAngle.z < -XM_PIDIV2 + (XM_PIDIV4 / 10.0f))
		{
			launchAngle.z = -XM_PIDIV2 + (XM_PIDIV4 / 10.0f);
		}

		m_rocketBody->SetRotation(launchAngle);
		m_rocketLauncher->Update();
	}
}

void Rocket::LaunchRocket()
{
	if (!m_rocketLaunched)
	{
		const auto launchAngle = m_rocketBody->GetRotationComponent()->GetRotationAt(0);

		//Turn the rocket angle to the launch angle we need
		const auto angle = XM_PIDIV2 + launchAngle.z;

		m_velocity = XMFLOAT2(m_initialVelocity * cos(angle), m_initialVelocity * sin(angle));
		m_angularVelocity = XMFLOAT2(cos(-angle), sin(-angle));

		const auto totalAngularMovement = angle + XM_PIDIV2;

		const auto v = m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y;

		auto totalTimeOfJourney = (v * sin(angle) + sqrt(((v * sin(angle)) * (v * sin(angle))) + 2.0f * (m_gravity * 3.0f))) / m_gravity;

		totalTimeOfJourney *= 2.0f;

		m_angularVelocity = XMFLOAT2(totalAngularMovement / totalTimeOfJourney, totalAngularMovement / totalTimeOfJourney);

		//totalTimeOfJourney = totalTimeOfJourney / (v * cos(angle));

		//m_angularVelocity = XMFLOAT2(totalAngularMovement / totalTimeOfJourney, totalAngularMovement / totalTimeOfJourney);
		//m_angularVelocity = XMFLOAT2(totalTimeOfJourney / totalAngularMovement, totalTimeOfJourney / totalAngularMovement);

		m_rocketLaunched = true;
		m_particleSystemActive = true;
	}
}

const bool Rocket::RocketLaunched() const
{
	return m_rocketLaunched;
}

const bool Rocket::ParticleSystemActive() const
{
	return m_particleSystemActive;
}



const XMFLOAT3& Rocket::GetLauncherPosition() const
{
	return m_initialLauncherPosition;
}

const XMFLOAT3& Rocket::GetLookAtRocketPosition()
{
	const auto rocketBodyPosition = m_rocketBody->GetPositionComponent()->GetPositionAt(0);
	const auto rocketBodyScale = m_rocketBody->GetScaleComponent()->GetScaleAt(0);

	m_lookAtRocketPosition = XMFLOAT3(rocketBodyPosition.x, rocketBodyPosition.y, rocketBodyPosition.z - rocketBodyScale.x);

	return m_lookAtRocketPosition;
}

const XMFLOAT3& Rocket::GetLookAtRocketConePosition()
{
	const auto rocketBodyScale = m_rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto rocketBodyRot = m_rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto rocketBodyPos = m_rocketBody->GetPositionComponent()->GetPositionAt(0);

	auto rocketMatrix = XMMatrixIdentity();

	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixScaling(rocketBodyScale.x, rocketBodyScale.y, rocketBodyScale.z));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rocketBodyRot.x, rocketBodyRot.y, rocketBodyRot.z)));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixTranslation(rocketBodyPos.x, rocketBodyPos.y, rocketBodyPos.z));

	auto posMatrix = XMMatrixIdentity();

	posMatrix = XMMatrixMultiply(posMatrix, XMMatrixTranslation(0.0f, 0.6f, 0.0f));

	posMatrix = posMatrix * rocketMatrix;

	auto lookAtRocketConeScale = XMVECTOR();
	auto lookAtRocketConeRotation = XMVECTOR();
	auto lookAtRocketConePosition = XMVECTOR();

	XMMatrixDecompose(&lookAtRocketConeScale, &lookAtRocketConeRotation, &lookAtRocketConePosition, posMatrix);

	auto conePositionFloat = XMFLOAT3();

	XMStoreFloat3(&conePositionFloat, lookAtRocketConePosition);

	m_lookAtRocketConePosition = XMFLOAT3(conePositionFloat.x, conePositionFloat.y, conePositionFloat.z - rocketBodyScale.x);

	return m_lookAtRocketConePosition;
}

const shared_ptr<GameObject> Rocket::GetRocketBody() const
{
	return m_rocketBody;
}

const shared_ptr<GameObject> Rocket::GetRocketCone() const
{
	return m_rocketCone;
}

const shared_ptr<GameObject> Rocket::GetRocketCap() const
{
	return m_rocketCap;
}

const shared_ptr<GameObject> Rocket::GetRocketLauncher() const
{
	return m_rocketLauncher;
}

const shared_ptr<Light>& Rocket::GetParticleSystemLight() const
{
	return m_particleSystemLight;
}

bool Rocket::CheckForTerrainCollision(const shared_ptr<Terrain>& terrain, XMFLOAT3& outCollisionPosition, float& outBlastRadius)
{
	const auto rocketBodyScale = m_rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto rocketBodyRot = m_rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto rocketBodyPos = m_rocketBody->GetPositionComponent()->GetPositionAt(0);

	auto rocketMatrix = XMMatrixIdentity();

	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixScaling(rocketBodyScale.x, rocketBodyScale.y, rocketBodyScale.z));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rocketBodyRot.x, rocketBodyRot.y, rocketBodyRot.z)));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixTranslation(rocketBodyPos.x, rocketBodyPos.y, rocketBodyPos.z));

	auto posMatrix = XMMatrixIdentity();

	posMatrix = XMMatrixMultiply(posMatrix, XMMatrixTranslation(0.0f, 0.6f, 0.0f));

	posMatrix = posMatrix * rocketMatrix;

	auto rocketConeScale = XMVECTOR();
	auto rocketConeRotation = XMVECTOR();
	auto rocketConePosition = XMVECTOR();

	XMMatrixDecompose(&rocketConeScale, &rocketConeRotation, &rocketConePosition, posMatrix);

	auto conePositionFloat = XMFLOAT3();

	XMStoreFloat3(&conePositionFloat, rocketConePosition);

	if (conePositionFloat.y < 0.0f)
	{
		//Collision true

		if (conePositionFloat.y < -200.0f)
		{
			//Reset if we have fallen too far
			ResetRocketState();
			return false;
		}

		auto terrainPositions = terrain->GetPositionComponent()->GetPositions();
		const auto terrainCubeRadius = terrain->GetScaleComponent()->GetScaleAt(0).x;

		for (unsigned int i = 0; i < terrainPositions.size(); i++)
		{
			auto distance = XMFLOAT3(terrainPositions[i].x - conePositionFloat.x, terrainPositions[i].y - conePositionFloat.y, terrainPositions[i].z - conePositionFloat.z);

			auto size = 0.0f;

			XMStoreFloat(&size, XMVector3Length(XMLoadFloat3(&distance)));

			//See if we collide with a single block and don't destroy within the blast radius

			auto radiusSum = XMVectorGetX(rocketConeScale) + terrainCubeRadius;

			if (size*size <= radiusSum * radiusSum)
			{
				outCollisionPosition = terrainPositions[i];
				outBlastRadius = m_blastRadius;

				terrain->GetPositionComponent()->TranslatePositionAt(XMFLOAT3(0.0f, -1000.0f, 0.0f), i);

				terrainPositions = terrain->GetPositionComponent()->GetPositions();

				for (unsigned int j = 0; j < terrainPositions.size(); j++)
				{
					distance = XMFLOAT3(terrainPositions[j].x - conePositionFloat.x, terrainPositions[j].y - conePositionFloat.y, terrainPositions[j].z - conePositionFloat.z);

					size = 0.0f;

					XMStoreFloat(&size, XMVector3Length(XMLoadFloat3(&distance)));

					//Destroy all blocks in the radius
					radiusSum = XMVectorGetX(rocketConeScale) + terrainCubeRadius + m_blastRadius;

					if (size*size <= radiusSum * radiusSum)
					{
						terrain->GetPositionComponent()->TranslatePositionAt(XMFLOAT3(0.0f, -500.0f, 0.0f), j);
					}
				}

				terrain->UpdateInstanceData();

				//Reset rocket
				ResetRocketState();

				//Return true and break out of loop
				return true;
			}
		}

		return false;
	}
}

void Rocket::ResetRocketState()
{
	m_rocketBody->SetPosition(m_initialLauncherPosition);
	m_rocketBody->SetRotation(m_initialLauncherRotation);

	m_rocketLaunched = false;
	m_particleSystemActive = false;
	m_changedParticleSystem = false;
	m_rocketLauncher->Update();
}


void Rocket::UpdateRocket(const float dt)
{
	if (m_rocketLaunched)
	{
		m_velocity = XMFLOAT2(m_velocity.x, (m_velocity.y + (m_gravity * dt)));
		m_angularVelocity = XMFLOAT2(m_angularVelocity.x, m_angularVelocity.y);

		if (m_velocity.y < 0.0f)
		{
			if (!m_changedParticleSystem)
			{
				m_particleSystemActive = false;
				m_changedParticleSystem = true;
			}

			auto rocketRotation = m_rocketBody->GetRotationComponent()->GetRotationAt(0);

			rocketRotation = XMFLOAT3(rocketRotation.x, rocketRotation.y, rocketRotation.z + m_angularVelocity.x);

			m_rocketBody->SetRotation(rocketRotation);
		}

		const auto rocketPosition = m_rocketBody->GetPositionComponent()->GetPositionAt(0);

		m_rocketBody->SetPosition(rocketPosition.x + m_velocity.x * dt, rocketPosition.y + m_velocity.y * dt, rocketPosition.z);
	}

	m_rocketBody->Update();
	m_rocketCone->Update();
	m_rocketCap->Update();
	m_fireJetParticleSystem->UpdateParticles(dt);
	m_coneFlameParticleSystem->UpdateParticles(dt);

	UpdateLightPosition();
}

void Rocket::UpdateLightPosition() const
{
	const auto rocketBodyScale = m_rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto rocketBodyRot = m_rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto rocketBodyPos = m_rocketBody->GetPositionComponent()->GetPositionAt(0);

	auto rocketMatrix = XMMatrixIdentity();

	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixScaling(rocketBodyScale.x, rocketBodyScale.y, rocketBodyScale.z));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rocketBodyRot.x, rocketBodyRot.y, rocketBodyRot.z)));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixTranslation(rocketBodyPos.x, rocketBodyPos.y, rocketBodyPos.z));

	auto posMatrix = XMMatrixIdentity();

	posMatrix = XMMatrixMultiply(posMatrix, XMMatrixTranslation(0.0f, -0.6f, 0.0f));

	posMatrix = posMatrix * rocketMatrix;

	auto lightScale = XMVECTOR();
	auto lightRotation = XMVECTOR();
	auto lightPosition = XMVECTOR();

	XMMatrixDecompose(&lightScale, &lightRotation, &lightPosition, posMatrix);

	posMatrix = XMMatrixIdentity();

	posMatrix = XMMatrixMultiply(posMatrix, XMMatrixTranslation(0.0f, -1.2f, 0.0f));

	posMatrix = posMatrix * rocketMatrix;

	auto lightPointPosition = XMVECTOR();
	XMMatrixDecompose(&lightScale, &lightRotation, &lightPointPosition, posMatrix);

	auto lightPositionFloat = XMFLOAT3();
	auto lightPointPositionFloat = XMFLOAT3();

	XMStoreFloat3(&lightPositionFloat, lightPosition);
	XMStoreFloat3(&lightPointPositionFloat, lightPointPosition);

	m_particleSystemLight->SetLightPosition(lightPositionFloat);
	m_particleSystemLight->SetLightPointPosition(lightPointPositionFloat);
}


bool Rocket::RenderRocket(const shared_ptr<D3DContainer>& d3dContainer, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) const
{
	auto result = true;

	result = m_rocketBody->Render(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition);

	if (!result)
	{
		return false;
	}

	result = m_rocketCone->Render(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition);

	if (!result)
	{
		return false;
	}

	result = m_rocketCap->Render(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition);

	if (!result)
	{
		return false;
	}

	result = m_rocketLauncher->Render(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition);

	if (!result)
	{
		return false;
	}

	if (m_particleSystemActive && m_rocketLaunched)
	{
		d3dContainer->DisableDepthStencil();
		d3dContainer->EnableAlphaBlending();

		result = m_fireJetParticleSystem->RenderFireJetParticleSystem(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, cameraPosition);

		d3dContainer->EnabledDepthStencil();
		d3dContainer->DisableAlphaBlending();

		if (!result)
		{
			return false;
		}
	}
	else if (m_rocketLaunched)
	{
		d3dContainer->DisableDepthStencil();
		d3dContainer->EnableAlphaBlending();

		result = m_coneFlameParticleSystem->RenderFireJetParticleSystem(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, cameraPosition);

		d3dContainer->EnabledDepthStencil();
		d3dContainer->DisableAlphaBlending();

		if (!result)
		{
			return false;
		}
	}

	return result;
}