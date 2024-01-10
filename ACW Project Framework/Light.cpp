#include "Light.h"


Light::Light() : m_orbit(false), m_directionalLight(false), m_lightPosition(XMFLOAT3()), m_lightRotation(XMFLOAT3()), m_lightPointPosition(XMFLOAT3()), m_lightDirection(XMFLOAT3()), m_ambientColour(XMFLOAT4()), m_diffuseColour(XMFLOAT4()), m_specularColour(XMFLOAT4()), m_specularPower(0.0f), m_lightViewMatrix(XMFLOAT4X4()), m_lightProjectionMatrix(XMFLOAT4X4()) {
	
}

//Light::Light(const Light& other) = default;

//Light::Light(Light&& other) noexcept = default;

Light::~Light() = default;

//Light& Light::operator=(const Light& other) = default;

//Light& Light::operator=(Light&& other) noexcept = default;

void Light::SetLightOrbit(const bool orbit)
{
	m_orbit = orbit;
}

void Light::SetDirectionalLight(const bool directionalLight)
{
	m_directionalLight = directionalLight;
}


void Light::SetLightPosition(const float x, const float y, const float z) {
	m_lightPosition = XMFLOAT3(x, y, z);
}

void Light::SetLightPosition(const XMFLOAT3& lightPosition) {
	m_lightPosition = lightPosition;
}

void Light::SetLightPointPosition(const float x, const float y, const float z) {
	m_lightPointPosition = XMFLOAT3(x, y, z);
}

void Light::SetLightPointPosition(const XMFLOAT3& lightPointPosition) {
	m_lightPointPosition = lightPointPosition;
}

void Light::SetAmbientColour(const float red, const float green, const float blue, const float alpha) {
	m_ambientColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::SetAmbientColour(const XMFLOAT4& ambientColour) {
	m_ambientColour = ambientColour;
}

void Light::SetDiffuseColour(const float red, const float green, const float blue, const float alpha) {
	m_diffuseColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::SetDiffuseColour(const XMFLOAT4& diffuseColour) {
	m_diffuseColour = diffuseColour;
}

void Light::SetSpecularColour(const float red, const float green, const float blue, const float alpha)
{
	m_specularColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::SetSpecularColour(const XMFLOAT4& specularColour) {
	m_specularColour = specularColour;
}

void Light::SetSpecularPower(const float specularPower)
{
	m_specularPower = specularPower;
}

void Light::GenerateLightProjectionMatrix(const float width, const float length, const float screenNear, const float screenDepth) {

	//m_lightProjectionMatrix = XMMatrixPerspectiveFovLH(static_cast<float>(XM_PI / 2.0f), 1, 1, 40);
	//XMStoreFloat4x4(&m_lightProjectionMatrix, XMMatrixOrthographicOffCenterLH(-45, 45, -45, 45, 1, 61));

	//XMStoreFloat4x4(&m_lightProjectionMatrix, XMMatrixOrthographicOffCenterLH(-45, 45, -45, 45, 1, 81));
	XMStoreFloat4x4(&m_lightProjectionMatrix, XMMatrixOrthographicOffCenterLH(-width, width, -length, 40, screenNear, (screenDepth * 2) - 5));

}

const bool Light::GetIsDirectionalLight() const
{
	return m_directionalLight;
}

const XMFLOAT3& Light::GetLightPosition() const {
	return m_lightPosition;
}

//const XMFLOAT3& Light::GetLightDirection() const {
//	return m_lightDirection;
//}

const XMFLOAT4& Light::GetAmbientColour() const
{
	return m_ambientColour;
}

const XMFLOAT4& Light::GetDiffuseColour() const {
	return m_diffuseColour;
}

const XMFLOAT4& Light::GetSpecularColour() const
{
	return m_specularColour;
}

float Light::GetSpecularPower() const
{
	return m_specularPower;
}

const XMMATRIX Light::GetLightViewMatrix() const
{
	return XMLoadFloat4x4(&m_lightViewMatrix);
}

const XMMATRIX Light::GetLightProjectionMatrix() const {
	return XMLoadFloat4x4(&m_lightProjectionMatrix);
}

void Light::SetLightOrbitDirection()
{
	XMStoreFloat3(&m_lightOrbitDirection, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&m_lightPointPosition), XMLoadFloat3(&m_lightPosition))));

	m_lightOrbitDirection = XMFLOAT3(abs(m_lightOrbitDirection.x), abs(m_lightOrbitDirection.y), abs(m_lightOrbitDirection.z));
}


void Light::UpdateLightVariables(const float dt)
{
	UpdateLightDirection();
	UpdateLightViewMatrix(dt);
}

void Light::UpdateLightDirection()
{
	XMStoreFloat3(&m_lightDirection, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&m_lightPointPosition), XMLoadFloat3(&m_lightPosition))));
}

void Light::UpdateLightViewMatrix(const float dt)
{
	auto parentMatrix = XMMatrixIdentity();

	const float pitch = m_lightOrbitDirection.z * (XM_PIDIV4 * dt);
	//const float yaw = m_lightDirection.y * (XM_PIDIV4 * dt);
	const float roll = m_lightOrbitDirection.x * (XM_PIDIV4 * dt);

	if (m_orbit)
	{
		parentMatrix = XMMatrixMultiply(parentMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(pitch, 0.0f, roll)));

	}

	auto worldMatrix = XMMatrixIdentity();
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixLookAtLH(XMLoadFloat3(&m_lightPosition), XMLoadFloat3(&m_lightPointPosition), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));

	XMStoreFloat4x4(&m_lightViewMatrix, XMMatrixMultiply(parentMatrix, worldMatrix));

	XMVECTOR outScale;
	XMVECTOR outRotation;
	XMVECTOR newLightPosition;

	XMMatrixDecompose(&outScale, &outRotation, &newLightPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_lightViewMatrix)));

	XMStoreFloat3(&m_lightPosition, newLightPosition);
}
