#include "LightManager.h"

LightManager::LightManager() = default;

LightManager::LightManager(const LightManager& other) = default;

LightManager::LightManager(LightManager&& other) noexcept = default;

LightManager::~LightManager() = default;

LightManager& LightManager::operator=(const LightManager& other) = default;

LightManager& LightManager::operator=(LightManager&& other) noexcept = default;

void LightManager::AddLight(const XMFLOAT3& lightPosition, const XMFLOAT3& lightPointPosition, const XMFLOAT4& ambientColour, const XMFLOAT4& diffuseColour, const XMFLOAT4& specularColour, const float specularPower, const float projectionScreenWidth, const float projectionScreenHeight, const float screenNear, const float screenDepth, const bool orbit, const bool directionalLight) {
	
	lights.push_back(make_shared<Light>());
	lights.back()->SetLightOrbit(orbit);
	lights.back()->SetDirectionalLight(directionalLight);
	lights.back()->SetLightPosition(lightPosition);
	lights.back()->SetLightPointPosition(lightPointPosition);
	lights.back()->SetAmbientColour(ambientColour);
	lights.back()->SetDiffuseColour(diffuseColour);
	lights.back()->SetSpecularColour(specularColour);
	lights.back()->SetSpecularPower(specularPower);
	lights.back()->GenerateLightProjectionMatrix(projectionScreenWidth, projectionScreenHeight, screenNear, screenDepth);
	lights.back()->SetLightOrbitDirection();

	lights.back()->UpdateLightVariables(0.0f);
}


const vector<shared_ptr<Light>>& LightManager::GetLightList() const {
	return lights;
}
