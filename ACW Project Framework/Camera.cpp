#include "Camera.h"
#include <memory>

#define _USE_MATH_DEFINES_

Camera::Camera() : positionX(0), positionY(0), positionZ(0), rotationX(0), rotationY(0), rotationZ(0)
{
}

Camera::Camera(const Camera& other) = default;

Camera::Camera(Camera&& other) = default;

Camera::~Camera() = default;

Camera& Camera::operator=(const Camera& other) = default;

//CameraClass& CameraClass::operator=(CameraClass&& other) noexcept = default;

XMFLOAT3 Camera::GetPosition() const {
	return XMFLOAT3(positionX, positionY, positionZ);
}

XMFLOAT3 Camera::GetRotation() const {
	return XMFLOAT3(rotationX, rotationY, rotationZ);
}

void Camera::SetPosition(const XMFLOAT3& position)
{
	positionX = position.x;
	positionY = position.y;
	positionZ = position.z;
}


void Camera::SetPosition(const float x, const float y, const float z) {
	positionX = x;
	positionY = y;
	positionZ = z;
}

void Camera::SetRotation(const float x, const float y, const float z) {
	rotationX = x;
	rotationY = y;
	rotationZ = z;
}

void Camera::AddPositionX(const float x)
{
	positionX += x;
}

void Camera::AddPositionY(const float y)
{
	positionY += y;
}

void Camera::AddPositionZ(const float z)
{
	positionZ += z;
}

void Camera::AddRotationX(const float x)
{
	rotationX += x;
}

void Camera::AddRotationY(const float y)
{
	rotationY += y;
}

void Camera::AddRotationZ(const float z)
{
	rotationZ += z;
}

void Camera::GetViewMatrix(XMMATRIX& viewMat) const {
	viewMat = XMLoadFloat4x4(&viewMatrix);
}

void Camera::Render() {
	const auto positionVector = XMVectorSet(positionX, positionY, positionZ, 0.0f);

	//const auto yaw = XMConvertToRadians(rotationX);
	const auto yaw = XMConvertToRadians(rotationX);
	const auto pitch = XMConvertToRadians(rotationY);
	const auto roll = XMConvertToRadians(rotationZ);

	const auto rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//Transform the upVector and lookAtVector by our rotation matrix so the camera is rotated at the origin
	const auto upVector = XMVector3TransformCoord(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);
	auto lookAtVector = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);

	//Translate camera to look at position
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//Create view matrix

	XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(positionVector, lookAtVector, upVector));
}
