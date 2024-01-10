#include "Scale.h"



Scale::Scale()
{
	
}

Scale::Scale(const XMFLOAT3& scale) {
	m_scales.push_back(scale);
}

Scale::Scale(const float x, const float y, const float z) {
	m_scales.emplace_back(XMFLOAT3(x, y, z));
}

Scale::Scale(const vector<XMFLOAT3>& scales)
{
	m_scales = scales;
}

Scale::Scale(const Scale& other) = default;

Scale::Scale(Scale&& other) noexcept = default;

Scale::~Scale() = default;

Scale& Scale::operator=(const Scale& other) = default;

const vector<XMFLOAT3>& Scale::GetScales() const
{
	return m_scales;
}

const XMFLOAT3& Scale::GetScaleAt(const int index) const
{
	return m_scales[index];
}

void Scale::AddScaleBack(const XMFLOAT3& scale)
{
	m_scales.push_back(scale);
}

void Scale::AddScaleBack(const float x, const float y, const float z)
{
	m_scales.emplace_back(XMFLOAT3(x, y, z));
}

void Scale::AddScaleFront(const XMFLOAT3& scale)
{
	m_scales.insert(m_scales.begin(), scale);
}

void Scale::AddScaleFront(const float x, const float y, const float z)
{
	m_scales.insert(m_scales.begin(), XMFLOAT3(x, y, z));
}

void Scale::RemoveScaleBack()
{
	m_scales.pop_back();
}

void Scale::SetScaleAt(const XMFLOAT3& newScale, const int index)
{
	m_scales[index] = newScale;
}

void Scale::SetScaleAt(const float x, const float y, const float z, const int index)
{
	m_scales[index] = XMFLOAT3(x, y, z);
}

void Scale::IncreaseAllScales(const XMFLOAT3& scalar)
{
	for (auto scale : m_scales)
	{
		scale = XMFLOAT3(scale.x + scalar.x, scale.y + scalar.y, scale.z + scalar.z);
	}
}

void Scale::IncreaseAllScales(const float x, const float y, const float z)
{
	for (auto scale : m_scales)
	{
		scale = XMFLOAT3(scale.x + x, scale.y + y, scale.z + z);
	}
}

void Scale::IncreaseScaleAt(const XMFLOAT3& scalar, const int index)
{
	m_scales[index] = XMFLOAT3(m_scales[index].x + scalar.x, m_scales[index].y + scalar.y, m_scales[index].z + scalar.z);
}

void Scale::IncreaseScaleAt(const float x, const float y, const float z, const int index)
{
	m_scales[index] = XMFLOAT3(m_scales[index].x + x, m_scales[index].y + y, m_scales[index].z + z);
}