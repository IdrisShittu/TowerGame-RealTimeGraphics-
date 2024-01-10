#include "Position.h"

Position::Position()
{
}

Position::Position(const XMFLOAT3& position) {
	m_positions.push_back(position);
}

Position::Position(const float x, const float y, const float z) {
	m_positions.emplace_back(XMFLOAT3(x, y, z));
}

Position::Position(const vector<XMFLOAT3>& positions)
{
	m_positions = positions;
}

Position::Position(const Position& other) = default;

Position::Position(Position&& other) noexcept = default;

Position::~Position() = default;

Position& Position::operator=(const Position& other) = default;

//Position& Position::operator=(Position&& other) noexcept = default;

const vector<XMFLOAT3>& Position::GetPositions() const
{
	return m_positions;
}

const XMFLOAT3& Position::GetPositionAt(const int index) const {
	return m_positions[index];
}

void Position::AddPositionBack(const XMFLOAT3& position)
{
	m_positions.push_back(position);
}

void Position::AddPositionBack(const float x, const float y, const float z)
{
	m_positions.emplace_back(XMFLOAT3(x, y, z));
}

void Position::AddPositionFront(const XMFLOAT3& position)
{
	m_positions.insert(m_positions.begin(), position);
}

void Position::AddPositionFront(const float x, const float y, const float z)
{
	m_positions.insert(m_positions.begin(), XMFLOAT3(x, y, z));
}

void Position::RemovePositionBack()
{
	m_positions.pop_back();
}

void Position::SetPositionAt(const XMFLOAT3& newPosition, const int index)
{
	m_positions[index] = newPosition;
}

void Position::SetPositionAt(const float x, const float y, const float z, const int index)
{
	m_positions[index] = XMFLOAT3(x, y, z);
}

void Position::TranslateAllPositions(const XMFLOAT3& translate)
{
	for (auto position : m_positions)
	{
		position = XMFLOAT3(position.x + translate.x, position.y + translate.y, position.z + translate.z);
	}
}

void Position::TranslateAllPositions(const float x, const float y, const float z)
{
	for (auto position : m_positions)
	{
		position = XMFLOAT3(position.x + x, position.y + y, position.z + z);
	}
}

void Position::TranslatePositionAt(const XMFLOAT3& translate, const int index)
{
	m_positions[index] = XMFLOAT3(m_positions[index].x + translate.x, m_positions[index].y + translate.y, m_positions[index].z + translate.z);
}

void Position::TranslatePositionAt(const float x, const float y, const float z, const int index)
{
	m_positions[index] = XMFLOAT3(m_positions[index].x + x, m_positions[index].y + y, m_positions[index].z + z);
}