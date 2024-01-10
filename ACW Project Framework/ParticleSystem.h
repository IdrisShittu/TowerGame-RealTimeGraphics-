#pragma once
#include "GameObject.h"
#include "ParticleShader.h"

//GameObject contains all the components types we need for the particle system

class ParticleSystem : public GameObject
{
public:
	//This is more of a stream, a recycling type of particle system
	ParticleSystem(ID3D11Device* const device, HWND const hwnd, const ModelType modelType, const XMFLOAT3& initialPosition, const XMFLOAT3& initialScale, const XMFLOAT3& finalScale, const XMFLOAT3& colourTint, const WCHAR* const textureName, const float transparency, const float lifeCycle, const float velocity, const int particleDensity, const shared_ptr<ResourceManager>& resourceManager);
	//This is an emitter type of particle system
	ParticleSystem(ID3D11Device* const device, HWND const hwnd, const XMFLOAT3& initialPosition, const XMFLOAT3& initialScale, const XMFLOAT3& scaleReduction, const XMFLOAT3& killScale, const XMFLOAT3& colourTint, const WCHAR* const textureName, const float transparency, const float spawnRate, const float velocity, const shared_ptr<ResourceManager>& resourceManager);
	//ParticleSystem(const ParticleSystem& other);
	//ParticleSystem(ParticleSystem&& other) noexcept;
	virtual ~ParticleSystem();

	ParticleSystem& operator = (const ParticleSystem& other);
	ParticleSystem& operator = (ParticleSystem&& other) noexcept;

	void UpdateParticles(const float dt);
	bool RenderParticles(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) const;

private:

	XMFLOAT3 m_initialPosition;
	XMFLOAT3 m_initialScale;
	XMFLOAT3 m_scaleReduction;
	XMFLOAT3 m_killScale;

	bool m_emitterType;

	float m_spawnRate;
	float m_elapsedTime;
	float m_lifeCycle;

	float m_velocity;

	float m_particleSpread;

};

