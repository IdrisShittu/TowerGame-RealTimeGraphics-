#include "LightManager.h"

LightManager::LightManager() = default;

LightManager::LightManager(const LightManager& other) = default;

LightManager::LightManager(LightManager&& other) noexcept = default;

LightManager::~LightManager() = default;

LightManager& LightManager::operator=(const LightManager& other) = default;

LightManager& LightManager::operator=(LightManager&& other) noexcept = default;

void LightManager::AddLight(const XMFLOAT3& lightPosition, const XMFLOAT3& lightPointPosition, const XMFLOAT4& ambientColour, const XMFLOAT4& diffuseColour, const XMFLOAT4& specularColour, const float specularPower, const float projectionScreenWidth, const float projectionScreenHeight, const float screenNear, const float screenDepth, const bool orbit, const bool directionalLight) {
	
	m_lights.push_back(make_shared<Light>());
	m_lights.back()->SetLightOrbit(orbit);
	m_lights.back()->SetDirectionalLight(directionalLight);
	m_lights.back()->SetLightPosition(lightPosition);
	m_lights.back()->SetLightPointPosition(lightPointPosition);
	m_lights.back()->SetAmbientColour(ambientColour);
	m_lights.back()->SetDiffuseColour(diffuseColour);
	m_lights.back()->SetSpecularColour(specularColour);
	m_lights.back()->SetSpecularPower(specularPower);
	m_lights.back()->GenerateLightProjectionMatrix(projectionScreenWidth, projectionScreenHeight, screenNear, screenDepth);
	m_lights.back()->SetLightOrbitDirection();

	m_lights.back()->UpdateLightVariables(0.0f);
}


const vector<shared_ptr<Light>>& LightManager::GetLightList() const {
	return m_lights;
}
