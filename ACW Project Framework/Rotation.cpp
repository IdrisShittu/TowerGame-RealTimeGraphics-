#include "Rotation.h"



Rotation::Rotation()
{

}

Rotation::Rotation(const XMFLOAT3& rotation) {
	m_rotations.push_back(rotation);
}

Rotation::Rotation(const float x, const float y, const float z) {
	m_rotations.emplace_back(XMFLOAT3(x, y, z));
}

Rotation::Rotation(const vector<XMFLOAT3>& rotations)
{
	m_rotations = rotations;
}

Rotation::Rotation(const Rotation& other) = default;

Rotation::Rotation(Rotation&& other) noexcept = default;

Rotation::~Rotation() = default;

Rotation& Rotation::operator=(const Rotation& other) = default;

//Rotation& Rotation::operator=(Rotation&& other) noexcept = default;

const vector<XMFLOAT3>& Rotation::GetRotations() const
{
	return m_rotations;
}

const XMFLOAT3& Rotation::GetRotationAt(const int index) const
{
	return m_rotations[index];
}

void Rotation::AddRotationBack(const XMFLOAT3& rotation)
{
	m_rotations.push_back(rotation);
}

void Rotation::AddRotationBack(const float x, const float y, const float z)
{
	m_rotations.emplace_back(XMFLOAT3(x, y, z));
}

void Rotation::AddRotationFront(const XMFLOAT3& rotation)
{
	m_rotations.insert(m_rotations.begin(), rotation);
}

void Rotation::AddRotationFront(const float x, const float y, const float z)
{
	m_rotations.insert(m_rotations.begin(), XMFLOAT3(x, y, z));
}

void Rotation::RemoveRotationBack()
{
	m_rotations.pop_back();
}

void Rotation::SetRotationAt(const XMFLOAT3& rotation, const int index)
{
	m_rotations[index] = rotation;
}

void Rotation::SetRotationAt(const float x, const float y, const float z, const int index)
{
	m_rotations[index] = XMFLOAT3(x, y, z);
}