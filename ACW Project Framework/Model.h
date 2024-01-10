#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <fstream>

#include "Texture.h"
#include "ResourceManager.h"

using namespace DirectX;
using namespace std;

class Model
{
public:
	Model(ID3D11Device* const device, const char* const modelFileName, const shared_ptr<ResourceManager>& resourceManager);
	Model(ID3D11Device* const device, const char* const modelFileName, const shared_ptr<ResourceManager>& resourceManager, const vector<XMFLOAT3> &scales, const vector<XMFLOAT3> &rotations, const vector<XMFLOAT3> &positions);

	Model(const Model& other); // Copy Constructor
	Model(Model && other) noexcept; // Move Constructor
	~Model(); // Destructor

	Model& operator = (const Model& other); // Copy Assignment Operator
	Model& operator = (Model&& other) noexcept; // Move Assignment Operator

	void Update(const vector<XMFLOAT3> &scales, const vector<XMFLOAT3> &rotations, const vector<XMFLOAT3> &positions, const XMMATRIX& parentMatrix);
	bool Render(ID3D11DeviceContext* const deviceContext);

	int GetIndexCount() const;
	int GetInstanceCount() const;

	bool GetInitializationState() const;

private:
	struct InstanceType
	{
		XMMATRIX worldMatrix;
	};

	bool m_initializationFailed;
	bool m_bufferDescriptionSizeChange = false;
	bool m_updateInstanceBuffer;

	int m_sizeOfVertexType = 0;

	int m_indexCount = 0;
	int m_instanceCount;

	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	ID3D11Buffer* m_instanceBuffer = nullptr;

	InstanceType* m_instances = nullptr;

	shared_ptr<D3D11_BUFFER_DESC> m_instanceBufferDescription;
	shared_ptr<D3D11_SUBRESOURCE_DATA> m_instanceData;
};