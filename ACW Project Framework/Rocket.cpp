#include "Rocket.h"

Rocket::Rocket(ID3D11Device* const device, const XMFLOAT3& position, const XMFLOAT3& rotation, const XMFLOAT3& scale, const shared_ptr<ShaderManager>& shaderManager, const shared_ptr<ResourceManager>& resourceManager) : initializationFailed(false), rocketLaunched(false), changedParticleSystem(false), particleSystemActive(false), blastRadius(4.0f), initialVelocity(25.0f), gravity(-9.81f), velocity(XMFLOAT2()), angularVelocity(XMFLOAT2()), initialLauncherPosition(XMFLOAT3()), initialLauncherRotation(XMFLOAT3()), lookAtRocketPosition(XMFLOAT3()), lookAtRocketConePosition(XMFLOAT3()), rocketCone(nullptr), rocketBody(nullptr), rocketCap(nullptr), rocketLauncher(nullptr), particleSystemLight(nullptr), fireJetParticleSystem(nullptr), coneFlameParticleSystem(nullptr)
{
	initialLauncherPosition = position;
	initialLauncherRotation = rotation;

	vector<const WCHAR*> textureNames;
	
	textureNames.push_back(L"BaseColour.dds");
	textureNames.push_back(L"BaseNormal.dds");
	textureNames.push_back(L"BaseSpecular.dds");
	textureNames.push_back(L"BaseDisplacement.dds");

	rocketBody = make_shared<GameObject>();
	rocketBody->AddPositionComponent(position);
	rocketBody->AddRotationComponent(rotation);
	rocketBody->AddScaleComponent(1.0f * scale.x, 6.0f * scale.y, 1.0f * scale.z);
	rocketBody->AddModelComponent(device, ModelType::LowPolyCylinder, resourceManager);
	rocketBody->AddTextureComponent(device, textureNames, resourceManager);
	rocketBody->SetShaderComponent(shaderManager->GetTextureDisplacementShader());
	rocketBody->SetTessellationVariables(2.0f, 10.0f, 64.0f, 1.0f);
	rocketBody->SetDisplacementVariables(20.0f, 0.0f, 6.0f, 0.18f);

	if (rocketBody->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(nullptr, "Could not initialize the rocket body game object.", "Error", MB_OK);
		return;
	}

	rocketCap = make_shared<GameObject>();
	rocketCap->AddPositionComponent(0.0f, -(3.0f * scale.y), 0.0f);
	rocketCap->AddRotationComponent(0.0f, 0.0f, 0.0f);
	rocketCap->AddScaleComponent(0.92f, 0.1f, 0.92f);
	rocketCap->AddModelComponent(device, ModelType::Sphere, resourceManager);
	rocketCap->AddTextureComponent(device, textureNames, resourceManager);
	rocketCap->SetShaderComponent(shaderManager->GetTextureDisplacementShader());
	rocketCap->AddParentGameObject(rocketBody);

	if (rocketCap->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(nullptr, "Could not initialize the rocket cap game object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	textureNames.push_back(L"MetalMeshColour.dds");
	textureNames.push_back(L"MetalMeshNormal.dds");
	textureNames.push_back(L"MetalMeshSpecular.dds");
	textureNames.push_back(L"MetalMeshDisplacement.dds");

	rocketLauncher = make_shared<GameObject>();
	rocketLauncher->AddPositionComponent((1.3f * scale.x), 0.0f, 0.0f);
	rocketLauncher->AddRotationComponent(0.0f, 0.0f, 0.0f);
	rocketLauncher->AddScaleComponent(0.3f * scale.x, 8.0f * scale.y, 0.3f * scale.z);
	rocketLauncher->AddModelComponent(device, ModelType::LowPolyCylinder, resourceManager);
	rocketLauncher->AddTextureComponent(device, textureNames, resourceManager);
	rocketLauncher->SetShaderComponent(shaderManager->GetTextureDisplacementShader());
	rocketLauncher->SetTessellationVariables(2.0f, 10.0f, 64.0f, 1.0f);
	rocketLauncher->SetDisplacementVariables(20.0f, 0.0f, 6.0f, 0.18f);
	rocketLauncher->AddParentGameObject(rocketBody);

	if (rocketLauncher->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(nullptr, "Could not initialize the rocket body game object.", "Error", MB_OK);
		return;
	}

	textureNames.clear();
	textureNames.push_back(L"skybox.dds");

	rocketCone = make_shared<GameObject>();
 	rocketCone->AddPositionComponent(0.0f, (3.0f * scale.y + 1.0f), 0.0f);
	rocketCone->AddRotationComponent(0.0f, 0.0f, 0.0f);
	rocketCone->AddScaleComponent(1.0f, 2.0f, 1.0f);
	rocketCone->AddModelComponent(device, ModelType::Cone, resourceManager);
	rocketCone->AddTextureComponent(device, textureNames, resourceManager);
	rocketCone->SetShaderComponent(shaderManager->GetReflectionShader());
	rocketCone->SetTessellationVariables(5.0f, 20.0f, 8.0f, 1.0f);
	rocketCone->AddParentGameObject(rocketBody);

	if (rocketCone->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(nullptr, "Could not initialize the rocket cone game object", "Error", MB_OK);
		return;
	}

	const auto rocketBodyScale = rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto rocketBodyRot = rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto rocketBodyPos = rocketBody->GetPositionComponent()->GetPositionAt(0);

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

	particleSystemLight = make_shared<Light>();
	particleSystemLight->SetLightOrbit(false);
	particleSystemLight->SetLightPosition(lightPositionFloat);
	particleSystemLight->SetLightPointPosition(lightPointPositionFloat);
	particleSystemLight->SetAmbientColour(0.15f, 0.15f, 0.15f, 1.0f);
	particleSystemLight->SetDiffuseColour(0.8f, 0.3f, 0.0f, 1.0f);
	particleSystemLight->SetSpecularColour(0.8f, 0.3f, 0.0f, 1.0f);
	particleSystemLight->SetSpecularPower(8.0f);
	particleSystemLight->GenerateLightProjectionMatrix(45.0f, 45.0f, 0.1f, 1000.0f);
	particleSystemLight->UpdateLightVariables(0.0f);

	fireJetParticleSystem = make_shared<FireJetParticleSystem>(device, nullptr, ModelType::Quad, XMFLOAT3(0.0f, -(3.0f * scale.y), 0.0f), XMFLOAT3(1.5f, 1.5f, 1.5f), XMFLOAT3(0.4f, 0.4f, 0.4f), 0.5f, 5.0f, 5.6f, 60, resourceManager);
	fireJetParticleSystem->AddParentGameObject(rocketBody);

	coneFlameParticleSystem = make_shared<FireJetParticleSystem>(device, nullptr, ModelType::SphereInverted, XMFLOAT3(0.0f, (4.0f * scale.y), 0.0f), XMFLOAT3(1.5f, 1.5f, 1.5f), XMFLOAT3(1.0f, 1.0f, 1.0f), 0.8f, 5.0f, 4.0f, 10, resourceManager);
	coneFlameParticleSystem->AddParentGameObject(rocketBody);

	rocketLauncher->Update();
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
	if (!rocketLaunched)
	{
		auto launchAngle = rocketBody->GetRotationComponent()->GetRotationAt(0);

		launchAngle.z -= -XM_PIDIV4 / 30.0f;

		if (launchAngle.z > 0.0f)
		{
			launchAngle.z = 0.0f;
		}

		rocketBody->SetRotation(launchAngle);
		rocketLauncher->Update();
	}
}

void Rocket::AdjustRotationRight() const
{
	if (!rocketLaunched)
	{
		auto launchAngle = rocketBody->GetRotationComponent()->GetRotationAt(0);

		launchAngle.z += -XM_PIDIV4 / 30.0f;

		if (launchAngle.z < -XM_PIDIV2 + (XM_PIDIV4 / 10.0f))
		{
			launchAngle.z = -XM_PIDIV2 + (XM_PIDIV4 / 10.0f);
		}

		rocketBody->SetRotation(launchAngle);
		rocketLauncher->Update();
	}
}

void Rocket::LaunchRocket()
{
	if (!rocketLaunched)
	{
		const auto launchAngle = rocketBody->GetRotationComponent()->GetRotationAt(0);

		//Turn the rocket angle to the launch angle we need
		const auto angle = XM_PIDIV2 + launchAngle.z;

		velocity = XMFLOAT2(initialVelocity * cos(angle), initialVelocity * sin(angle));
		angularVelocity = XMFLOAT2(cos(-angle), sin(-angle));

		const auto totalAngularMovement = angle + XM_PIDIV2;

		const auto v = velocity.x * velocity.x + velocity.y * velocity.y;

		auto totalTimeOfJourney = (v * sin(angle) + sqrt(((v * sin(angle)) * (v * sin(angle))) + 2.0f * (gravity * 3.0f))) / gravity;

		totalTimeOfJourney *= 2.0f;

		angularVelocity = XMFLOAT2(totalAngularMovement / totalTimeOfJourney, totalAngularMovement / totalTimeOfJourney);

		//totalTimeOfJourney = totalTimeOfJourney / (v * cos(angle));

		//angularVelocity = XMFLOAT2(totalAngularMovement / totalTimeOfJourney, totalAngularMovement / totalTimeOfJourney);
		//angularVelocity = XMFLOAT2(totalTimeOfJourney / totalAngularMovement, totalTimeOfJourney / totalAngularMovement);

		rocketLaunched = true;
		particleSystemActive = true;
	}
}

const bool Rocket::RocketLaunched() const
{
	return rocketLaunched;
}

const bool Rocket::ParticleSystemActive() const
{
	return particleSystemActive;
}



const XMFLOAT3& Rocket::GetLauncherPosition() const
{
	return initialLauncherPosition;
}

const XMFLOAT3& Rocket::GetLookAtRocketPosition()
{
	const auto rocketBodyPosition = rocketBody->GetPositionComponent()->GetPositionAt(0);
	const auto rocketBodyScale = rocketBody->GetScaleComponent()->GetScaleAt(0);

	lookAtRocketPosition = XMFLOAT3(rocketBodyPosition.x, rocketBodyPosition.y, rocketBodyPosition.z - rocketBodyScale.x);

	return lookAtRocketPosition;
}

const XMFLOAT3& Rocket::GetLookAtRocketConePosition()
{
	const auto rocketBodyScale = rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto rocketBodyRot = rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto rocketBodyPos = rocketBody->GetPositionComponent()->GetPositionAt(0);

	auto rocketMatrix = XMMatrixIdentity();

	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixScaling(rocketBodyScale.x, rocketBodyScale.y, rocketBodyScale.z));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rocketBodyRot.x, rocketBodyRot.y, rocketBodyRot.z)));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixTranslation(rocketBodyPos.x, rocketBodyPos.y, rocketBodyPos.z));

	auto posMatrix = XMMatrixIdentity();

	posMatrix = XMMatrixMultiply(posMatrix, XMMatrixTranslation(0.0f, 0.6f, 0.0f));

	posMatrix = posMatrix * rocketMatrix;

	auto lookAtRocketConeScale = XMVECTOR();
	auto lookAtRocketConeRotation = XMVECTOR();
	auto lookAtRocketConePos = XMVECTOR();

	XMMatrixDecompose(&lookAtRocketConeScale, &lookAtRocketConeRotation, &lookAtRocketConePos, posMatrix);

	auto conePositionFloat = XMFLOAT3();

	XMStoreFloat3(&conePositionFloat, lookAtRocketConePos);

	lookAtRocketConePosition = XMFLOAT3(conePositionFloat.x, conePositionFloat.y, conePositionFloat.z - rocketBodyScale.x);

	return lookAtRocketConePosition;
}

const shared_ptr<GameObject> Rocket::GetRocketBody() const
{
	return rocketBody;
}

const shared_ptr<GameObject> Rocket::GetRocketCone() const
{
	return rocketCone;
}

const shared_ptr<GameObject> Rocket::GetRocketCap() const
{
	return rocketCap;
}

const shared_ptr<GameObject> Rocket::GetRocketLauncher() const
{
	return rocketLauncher;
}

const shared_ptr<Light>& Rocket::GetParticleSystemLight() const
{
	return particleSystemLight;
}

bool Rocket::CheckForTerrainCollision(const shared_ptr<Terrain>& terrain, XMFLOAT3& outCollisionPosition, float& outBlastRadius)
{
	const auto rocketBodyScale = rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto rocketBodyRot = rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto rocketBodyPos = rocketBody->GetPositionComponent()->GetPositionAt(0);

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
				outBlastRadius = blastRadius;

				terrain->GetPositionComponent()->TranslatePositionAt(XMFLOAT3(0.0f, -1000.0f, 0.0f), i);

				terrainPositions = terrain->GetPositionComponent()->GetPositions();

				for (unsigned int j = 0; j < terrainPositions.size(); j++)
				{
					distance = XMFLOAT3(terrainPositions[j].x - conePositionFloat.x, terrainPositions[j].y - conePositionFloat.y, terrainPositions[j].z - conePositionFloat.z);

					size = 0.0f;

					XMStoreFloat(&size, XMVector3Length(XMLoadFloat3(&distance)));

					//Destroy all blocks in the radius
					radiusSum = XMVectorGetX(rocketConeScale) + terrainCubeRadius + blastRadius;

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
	rocketBody->SetPosition(initialLauncherPosition);
	rocketBody->SetRotation(initialLauncherRotation);

	rocketLaunched = false;
	particleSystemActive = false;
	changedParticleSystem = false;
	rocketLauncher->Update();
}


void Rocket::UpdateRocket(const float dt)
{
	if (rocketLaunched)
	{
		velocity = XMFLOAT2(velocity.x, (velocity.y + (gravity * dt)));
		angularVelocity = XMFLOAT2(angularVelocity.x, angularVelocity.y);

		if (velocity.y < 0.0f)
		{
			if (!changedParticleSystem)
			{
				particleSystemActive = false;
				changedParticleSystem = true;
			}

			auto rocketRotation = rocketBody->GetRotationComponent()->GetRotationAt(0);

			rocketRotation = XMFLOAT3(rocketRotation.x, rocketRotation.y, rocketRotation.z + angularVelocity.x);

			rocketBody->SetRotation(rocketRotation);
		}

		const auto rocketPosition = rocketBody->GetPositionComponent()->GetPositionAt(0);

		rocketBody->SetPosition(rocketPosition.x + velocity.x * dt, rocketPosition.y + velocity.y * dt, rocketPosition.z);
	}

	rocketBody->Update();
	rocketCone->Update();
	rocketCap->Update();
	fireJetParticleSystem->UpdateParticles(dt);
	coneFlameParticleSystem->UpdateParticles(dt);

	UpdateLightPosition();
}

void Rocket::UpdateLightPosition() const
{
	const auto rocketBodyScale = rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto rocketBodyRot = rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto rocketBodyPos = rocketBody->GetPositionComponent()->GetPositionAt(0);

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

	particleSystemLight->SetLightPosition(lightPositionFloat);
	particleSystemLight->SetLightPointPosition(lightPointPositionFloat);
}


bool Rocket::RenderRocket(const shared_ptr<D3DContainer>& d3dContainer, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) const
{
	auto result = true;

	result = rocketBody->Render(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition);

	if (!result)
	{
		return false;
	}

	result = rocketCone->Render(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition);

	if (!result)
	{
		return false;
	}

	result = rocketCap->Render(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition);

	if (!result)
	{
		return false;
	}

	result = rocketLauncher->Render(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition);

	if (!result)
	{
		return false;
	}

	if (particleSystemActive && rocketLaunched)
	{
		d3dContainer->DisableDepthStencil();
		d3dContainer->EnableAlphaBlending();

		result = fireJetParticleSystem->RenderFireJetParticleSystem(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, cameraPosition);

		d3dContainer->EnabledDepthStencil();
		d3dContainer->DisableAlphaBlending();

		if (!result)
		{
			return false;
		}
	}
	else if (rocketLaunched)
	{
		d3dContainer->DisableDepthStencil();
		d3dContainer->EnableAlphaBlending();

		result = coneFlameParticleSystem->RenderFireJetParticleSystem(d3dContainer->GetDeviceContext(), viewMatrix, projectionMatrix, cameraPosition);

		d3dContainer->EnabledDepthStencil();
		d3dContainer->DisableAlphaBlending();

		if (!result)
		{
			return false;
		}
	}

	return result;
}