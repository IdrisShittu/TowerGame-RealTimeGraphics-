#include "ParticleSystemManager.h"

ParticleSystemManager::ParticleSystemManager() : explosion(false), smokeRenderCount(0), explosionRenderCount(0), timeSinceExplosion(0.0f), smokeParticleSystems(), fireJetParticleSystems(), explosionLights()
{
}

ParticleSystemManager::ParticleSystemManager(const ParticleSystemManager& other) = default;

ParticleSystemManager::ParticleSystemManager(ParticleSystemManager&& other) noexcept = default;

ParticleSystemManager::~ParticleSystemManager()
{
}

ParticleSystemManager& ParticleSystemManager::operator=(const ParticleSystemManager& other) = default;

ParticleSystemManager& ParticleSystemManager::operator=(ParticleSystemManager&& other) noexcept = default;

const bool ParticleSystemManager::ExplosionExists() const
{
	return explosion;
}

const vector<shared_ptr<Light>>& ParticleSystemManager::GetLights() const
{
	return explosionLights;
}

void ParticleSystemManager::ResetParticleSystems()
{
	smokeRenderCount = 0;
}

void ParticleSystemManager::GenerateExplosion(ID3D11Device* const device, const XMFLOAT3& explosionPosition, const float blastRadius, const shared_ptr<ResourceManager>& resourceManager)
{
	if (blastRadius > 0.0f)
	{
		fireJetParticleSystems.insert(fireJetParticleSystems.begin(), make_shared<FireJetParticleSystem>(device, nullptr, ModelType::Quad, explosionPosition, XMFLOAT3(blastRadius * 4, blastRadius * 4, blastRadius * 4), XMFLOAT3(0.2f, 0.2f, 0.2f), 0.5f, 4.0f, 5.6f, 40.0f, resourceManager));
		smokeParticleSystems.insert(smokeParticleSystems.begin(), make_shared<SmokeParticleSystem>(device, nullptr, explosionPosition, XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-0.8f, -0.8f, -0.8f), XMFLOAT3(14.0f, 14.0f, 14.0f), 0.2f, 1.0f, 1.5f, resourceManager));
		explosionLights.emplace_back(make_shared<Light>());

		explosionLights.back()->SetLightOrbit(false);
		explosionLights.back()->SetLightPosition(explosionPosition);
		explosionLights.back()->SetLightPointPosition(0.0f, 0.0f, 0.0f);
		explosionLights.back()->SetAmbientColour(0.15f, 0.15f, 0.15f, 1.0f);
		explosionLights.back()->SetDiffuseColour(1.6f, 0.6f, 0.0f, 1.0f);
		explosionLights.back()->SetSpecularColour(1.6f, 0.6f, 0.0f, 1.0f);
		explosionLights.back()->SetSpecularPower(14.0f);
		explosionLights.back()->GenerateLightProjectionMatrix(45.0f, 45.0f, 1.0f, 1000.0f);
		explosionLights.back()->UpdateLightVariables(0.0f);

		smokeRenderCount++;
		explosionRenderCount++;
		explosion = true;
	}
}

void ParticleSystemManager::Update(const float dt)
{
	if (explosion)
	{
		timeSinceExplosion += dt;

		if (timeSinceExplosion > 1.0f)
		{
			//Destroy firejetparticleat
			//fireJetParticleSystems.erase(fireJetParticleSystems.begin());
			//explosionLights.erase(explosionLights.begin());

			//fireJetParticleSystems.pop_back();
			explosionRenderCount--;
			explosionLights.pop_back();

			if (fireJetParticleSystems.size() > explosionRenderCount)
			{
				explosion = false;
			}

			timeSinceExplosion = 0.0f;
		}
	}

	for (unsigned int i = 0; i < smokeRenderCount; i++)
	{
		smokeParticleSystems[i]->UpdateSmokeParticleSystem(dt);
	}

	for (unsigned int i = 0; i < explosionRenderCount; i++)
	{
		fireJetParticleSystems[i]->UpdateFireJetParticleSystem(dt); 
	}
}

bool ParticleSystemManager::Render(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) const
{
	for (unsigned int i = 0; i < smokeRenderCount; i++)
		if (!smokeParticleSystems[i]->RenderSmokeParticleSystem(deviceContext, viewMatrix, projectionMatrix, cameraPosition))
			return false;

	for (unsigned int i = 0; i < explosionRenderCount; i++)
		if (!fireJetParticleSystems[i]->RenderFireJetParticleSystem(deviceContext, viewMatrix, projectionMatrix, cameraPosition))
			return false;

	return true;
}




