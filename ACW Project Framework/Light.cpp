#include "Light.h"


Light::Light() : orbit(false), directionalLight(false), lightPosition(XMFLOAT3()), lightRotation(XMFLOAT3()), lightPointPosition(XMFLOAT3()), lightDirection(XMFLOAT3()), ambientColour(XMFLOAT4()), diffuseColour(XMFLOAT4()), specularColour(XMFLOAT4()), specularPower(0.0f), lightViewMatrix(XMFLOAT4X4()), lightProjectionMatrix(XMFLOAT4X4()) {
	
}

//Light::Light(const Light& other) = default;

//Light::Light(Light&& other) noexcept = default;

Light::~Light() = default;

//Light& Light::operator=(const Light& other) = default;

//Light& Light::operator=(Light&& other) noexcept = default;

void Light::SetLightOrbit(const bool orb)
{
	orbit = orb;
}

void Light::SetDirectionalLight(const bool dLight)
{
	directionalLight = dLight;
}


void Light::SetLightPosition(const float x, const float y, const float z) {
	lightPosition = XMFLOAT3(x, y, z);
}

void Light::SetLightPosition(const XMFLOAT3& lPosition) {
	lightPosition = lPosition;
}

void Light::SetLightPointPosition(const float x, const float y, const float z) {
	lightPointPosition = XMFLOAT3(x, y, z);
}

void Light::SetLightPointPosition(const XMFLOAT3& lPPosition) {
	lightPointPosition = lPPosition;
}

void Light::SetAmbientColour(const float red, const float green, const float blue, const float alpha) {
	ambientColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::SetAmbientColour(const XMFLOAT4& ambColour) {
	ambientColour = ambColour;
}

void Light::SetDiffuseColour(const float red, const float green, const float blue, const float alpha) {
	diffuseColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::SetDiffuseColour(const XMFLOAT4& diffColour) {
	diffuseColour = diffColour;
}

void Light::SetSpecularColour(const float red, const float green, const float blue, const float alpha)
{
	specularColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::SetSpecularColour(const XMFLOAT4& spColour) {
	specularColour = spColour;
}

void Light::SetSpecularPower(const float spPower)
{
	specularPower = spPower;
}

void Light::GenerateLightProjectionMatrix(const float width, const float length, const float screenNear, const float screenDepth) {

	//lightProjectionMatrix = XMMatrixPerspectiveFovLH(static_cast<float>(XPI / 2.0f), 1, 1, 40);
	//XMStoreFloat4x4(&lightProjectionMatrix, XMMatrixOrthographicOffCenterLH(-45, 45, -45, 45, 1, 61));

	//XMStoreFloat4x4(&lightProjectionMatrix, XMMatrixOrthographicOffCenterLH(-45, 45, -45, 45, 1, 81));
	XMStoreFloat4x4(&lightProjectionMatrix, XMMatrixOrthographicOffCenterLH(-width, width, -length, 40, screenNear, (screenDepth * 2) - 5));

}

const bool Light::GetIsDirectionalLight() const
{
	return directionalLight;
}

const XMFLOAT3& Light::GetLightPosition() const {
	return lightPosition;
}

//const XMFLOAT3& Light::GetLightDirection() const {
//	return lightDirection;
//}

const XMFLOAT4& Light::GetAmbientColour() const
{
	return ambientColour;
}

const XMFLOAT4& Light::GetDiffuseColour() const {
	return diffuseColour;
}

const XMFLOAT4& Light::GetSpecularColour() const
{
	return specularColour;
}

float Light::GetSpecularPower() const
{
	return specularPower;
}

const XMMATRIX Light::GetLightViewMatrix() const
{
	return XMLoadFloat4x4(&lightViewMatrix);
}

const XMMATRIX Light::GetLightProjectionMatrix() const {
	return XMLoadFloat4x4(&lightProjectionMatrix);
}

void Light::SetLightOrbitDirection()
{
	XMStoreFloat3(&lightOrbitDirection, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&lightPointPosition), XMLoadFloat3(&lightPosition))));

	lightOrbitDirection = XMFLOAT3(abs(lightOrbitDirection.x), abs(lightOrbitDirection.y), abs(lightOrbitDirection.z));
}


void Light::UpdateLightVariables(const float dt)
{
	UpdateLightDirection();
	UpdateLightViewMatrix(dt);
}

void Light::UpdateLightDirection()
{
	XMStoreFloat3(&lightDirection, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&lightPointPosition), XMLoadFloat3(&lightPosition))));
}

void Light::UpdateLightViewMatrix(const float dt)
{
	auto parentMatrix = XMMatrixIdentity();

	const float pitch = lightOrbitDirection.z * (XM_PIDIV4 * dt);
	//const float yaw = lightDirection.y * (XM_PIDIV4 * dt);
	const float roll = lightOrbitDirection.x * (XM_PIDIV4 * dt);

	if (orbit)
	{
		parentMatrix = XMMatrixMultiply(parentMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(pitch, 0.0f, roll)));

	}

	auto worldMatrix = XMMatrixIdentity();
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixLookAtLH(XMLoadFloat3(&lightPosition), XMLoadFloat3(&lightPointPosition), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));

	XMStoreFloat4x4(&lightViewMatrix, XMMatrixMultiply(parentMatrix, worldMatrix));

	XMVECTOR outScale;
	XMVECTOR outRotation;
	XMVECTOR newLightPosition;

	XMMatrixDecompose(&outScale, &outRotation, &newLightPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&lightViewMatrix)));

	XMStoreFloat3(&lightPosition, newLightPosition);
}
