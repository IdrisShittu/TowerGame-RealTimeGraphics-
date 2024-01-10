#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ID3D11Device* const device, HWND const hwnd, const ModelType modelType, const XMFLOAT3& initialPosition, const XMFLOAT3& initialScale, const XMFLOAT3& finalScale, const XMFLOAT3& colourTint, const WCHAR* const textureName, const float transparency, const float lifeCycle, const float velocity, const int particleDensity, const shared_ptr<ResourceManager>& resourceManager) : m_initialPosition(initialPosition), m_initialScale(initialScale), m_scaleReduction(XMFLOAT3()), m_killScale(XMFLOAT3()), m_emitterType(false), m_spawnRate(0.0f), m_elapsedTime(0.0f), m_lifeCycle(lifeCycle), m_velocity(velocity), m_particleSpread(lifeCycle / particleDensity)
{
	auto positions = vector<XMFLOAT3>();
	auto scales = vector<XMFLOAT3>();

	//Add particle instances
	for (auto i = 1; i <= particleDensity; i++)
	{
		positions.emplace_back(XMFLOAT3(m_initialPosition.x, m_initialPosition.y - (i * m_particleSpread), m_initialPosition.z));

		auto particleScale = XMFLOAT3(initialScale.x - ((initialScale.x - finalScale.x) / particleDensity) * i, initialScale.y - ((initialScale.y - finalScale.y) / particleDensity) * i, initialScale.z - ((initialScale.z - finalScale.z) / particleDensity) * i);

		//scales.emplace_back(initialScale - ((initialScale - finalScale) / particleDensity) * i);
		scales.emplace_back(particleScale);
	}

	AddPositionComponent(positions);
	AddScaleComponent(scales);
	AddRotationComponent(0.0f, 0.0f, 0.0f);

	vector<const WCHAR*> textureNames;
	textureNames.push_back(textureName);

	AddModelComponent(device, modelType, resourceManager);
	AddTextureComponent(device, textureNames, resourceManager);

	const auto particleShader = make_shared<ParticleShader>(device, hwnd);

	particleShader->SetParticleParameters(colourTint, transparency);

	SetShaderComponent(particleShader);
}

ParticleSystem::ParticleSystem(ID3D11Device* const device, HWND const hwnd, const XMFLOAT3& initialPosition, const XMFLOAT3& initialScale, const XMFLOAT3& scaleReduction, const XMFLOAT3& killScale, const XMFLOAT3& colourTint, const WCHAR* const textureName, const float transparency, const float spawnRate, const float velocity, const shared_ptr<ResourceManager>& resourceManager) : m_initialPosition(initialPosition), m_initialScale(initialScale), m_scaleReduction(scaleReduction), m_killScale(killScale), m_emitterType(true), m_spawnRate(spawnRate), m_elapsedTime(0.0f), m_lifeCycle(0.0f), m_velocity(velocity), m_particleSpread(0.0f)
{
	AddPositionComponent(m_initialPosition);
	AddScaleComponent(m_initialScale);
	AddRotationComponent(0.0f, 0.0f, 0.0f);

	vector<const WCHAR*> textureNames;
	textureNames.push_back(textureName);

	AddModelComponent(device, ModelType::Quad, resourceManager);
	AddTextureComponent(device, textureNames, resourceManager);

	const auto particleShader = make_shared<ParticleShader>(device, hwnd);

	particleShader->SetParticleParameters(colourTint, transparency);

	SetShaderComponent(particleShader);
}

//ParticleSystem::ParticleSystem(const ParticleSystem& other) = default;

//ParticleSystem::ParticleSystem(ParticleSystem&& other) noexcept = default;

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::UpdateParticles(const float dt)
{
	if (m_emitterType)
	{
		m_elapsedTime += dt;

		//Add a new particle if the spawn rate is reached
		if (m_elapsedTime > m_spawnRate)
		{
			GetPositionComponent()->AddPositionFront(m_initialPosition);
			GetScaleComponent()->AddScaleFront(m_initialScale);

			m_elapsedTime = 0.0f;
		}

		//Check if we need to remove any particles
		for (const auto scale : GetScaleComponent()->GetScales())
		{
			if (scale.x > m_killScale.x)
			{
				//Remove particle
				GetPositionComponent()->RemovePositionBack();
				GetScaleComponent()->RemoveScaleBack();
			}
		}

		auto positions = GetPositionComponent()->GetPositions();
		auto scales = GetScaleComponent()->GetScales();

		//Update particles
		for (unsigned int i = 0; i < positions.size(); i++)
		{
			positions[i].y = positions[i].y + abs(m_velocity * dt);
			//scales[i] = scales[i] - (XMFLOAT3(m_scaleReduction.x * dt, m_scaleReduction.y * dt, m_scaleReduction.z * dt));
			scales[i] = XMFLOAT3(scales[i].x - m_scaleReduction.x * dt, scales[i].y - m_scaleReduction.y * dt, scales[i].z - m_scaleReduction.z * dt);
		}

		SetPosition(positions);
		SetScale(scales);
	}
	else
	{
		auto positions = GetPositionComponent()->GetPositions();

		for (auto& position : positions)
		{
			position = XMFLOAT3(position.x + 0.0f * m_velocity * dt, position.y + -1.0f * m_velocity * dt, position.z + 0.0f * m_velocity * dt);

			//If the new updated position has reached the end of its lifecycle then we 
			if (position.y < m_initialPosition.y - m_lifeCycle)
			{
				positions.pop_back();
				positions.insert(positions.begin(), m_initialPosition);
			}
		}

		SetPosition(positions);
	}

	Update();
}

bool ParticleSystem::RenderParticles(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) const
{
	return Render(deviceContext, viewMatrix, projectionMatrix, {}, {}, cameraPosition);
}


