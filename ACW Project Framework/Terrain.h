#pragma once

#include "GameObject.h"

using namespace std;
using namespace DirectX;

class Terrain : public GameObject
{
public:
	Terrain(ID3D11Device* const device, const XMFLOAT3& voxelArea, const XMFLOAT3& cubeScale, const shared_ptr<Shader>& shader, const shared_ptr<ResourceManager>& resourceManager);
	//Terrain(const Terrain& other);
	//Terrain(Terrain&& other) noexcept;
	~Terrain();

	//Terrain& operator = (const Terrain& other);
	//Terrain& operator = (Terrain&& other) noexcept;

	void ResetTerrainState();

	void UpdateTerrain();
	bool RenderTerrain(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) const;

private:
	bool m_initializationFailed;

	vector<XMFLOAT3> m_initialTerrainPositions;
};

