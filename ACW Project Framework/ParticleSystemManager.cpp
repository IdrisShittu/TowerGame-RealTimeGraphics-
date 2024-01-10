#include "ParticleSystemManager.h"

ParticleSystemManager::ParticleSystemManager() : m_explosion(false), m_smokeRenderCount(0), m_explosionRenderCount(0), m_timeSinceExplosion(0.0f), m_smokeParticleSystems(), m_fireJetParticleSystems(), m_explosionLights()
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
	return m_explosion;
}

const vector<shared_ptr<Light>>& ParticleSystemManager::GetLights() const
{
	return m_explosionLights;
}

void ParticleSystemManager::ResetParticleSystems()
{
	m_smokeRenderCount = 0;
}

void ParticleSystemManager::GenerateExplosion(ID3D11Device* const device, const XMFLOAT3& explosionPosition, const float blastRadius, const shared_ptr<ResourceManager>& resourceManager)
{
	if (blastRadius > 0.0f)
	{
		m_fireJetParticleSystems.insert(m_fireJetParticleSystems.begin(), make_shared<FireJetParticleSystem>(device, nullptr, ModelType::Quad, explosionPosition, XMFLOAT3(blastRadius * 4, blastRadius * 4, blastRadius * 4), XMFLOAT3(0.2f, 0.2f, 0.2f), 0.5f, 4.0f, 5.6f, 40.0f, resourceManager));
		m_smokeParticleSystems.insert(m_smokeParticleSystems.begin(), make_shared<SmokeParticleSystem>(device, nullptr, explosionPosition, XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-0.8f, -0.8f, -0.8f), XMFLOAT3(14.0f, 14.0f, 14.0f), 0.2f, 1.0f, 1.5f, resourceManager));
		m_explosionLights.emplace_back(make_shared<Light>());

		m_explosionLights.back()->SetLightOrbit(false);
		m_explosionLights.back()->SetLightPosition(explosionPosition);
		m_explosionLights.back()->SetLightPointPosition(0.0f, 0.0f, 0.0f);
		m_explosionLights.back()->SetAmbientColour(0.15f, 0.15f, 0.15f, 1.0f);
		m_explosionLights.back()->SetDiffuseColour(1.6f, 0.6f, 0.0f, 1.0f);
		m_explosionLights.back()->SetSpecularColour(1.6f, 0.6f, 0.0f, 1.0f);
		m_explosionLights.back()->SetSpecularPower(14.0f);
		m_explosionLights.back()->GenerateLightProjectionMatrix(45.0f, 45.0f, 1.0f, 1000.0f);
		m_explosionLights.back()->UpdateLightVariables(0.0f);

		m_smokeRenderCount++;
		m_explosionRenderCount++;
		m_explosion = true;
	}
}

void ParticleSystemManager::Update(const float dt)
{
	if (m_explosion)
	{
		m_timeSinceExplosion += dt;

		if (m_timeSinceExplosion > 1.0f)
		{
			//Destroy firejetparticleat
			//m_fireJetParticleSystems.erase(m_fireJetParticleSystems.begin());
			//m_explosionLights.erase(m_explosionLights.begin());

			//m_fireJetParticleSystems.pop_back();
			m_explosionRenderCount--;
			m_explosionLights.pop_back();

			if (m_fireJetParticleSystems.size() > m_explosionRenderCount)
			{
				m_explosion = false;
			}

			m_timeSinceExplosion = 0.0f;
		}
	}

	for (unsigned int i = 0; i < m_smokeRenderCount; i++)
	{
		m_smokeParticleSystems[i]->UpdateSmokeParticleSystem(dt);
	}

	for (unsigned int i = 0; i < m_explosionRenderCount; i++)
	{
		m_fireJetParticleSystems[i]->UpdateFireJetParticleSystem(dt); 
	}
}

bool ParticleSystemManager::Render(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) const
{
	auto result = true;

	if (!result)
	{
		return false;
	}

	for (unsigned int i = 0; i < m_smokeRenderCount; i++)
	{
		result = m_smokeParticleSystems[i]->RenderSmokeParticleSystem(deviceContext, viewMatrix, projectionMatrix, cameraPosition);

		if (!result)
		{
			return false;
		}
	}

	for (unsigned int i = 0; i < m_explosionRenderCount; i++)
	{
		result = m_fireJetParticleSystems[i]->RenderFireJetParticleSystem(deviceContext, viewMatrix, projectionMatrix, cameraPosition);

		if (!result)
		{
			return false;
		}
	}

	return result;
}




