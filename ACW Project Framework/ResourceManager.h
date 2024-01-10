#pragma once
#include <map>
#include <memory>
#include <fstream>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include <DDSTextureLoader.h>

using namespace std;
using namespace DirectX;

class ResourceManager
{
public:
	ResourceManager();
	ResourceManager(const ResourceManager& other); // Copy Constructor
	//ResourceManager(ResourceManager&& other) noexcept; // Move Constructor
	~ResourceManager();

	ResourceManager& operator = (const ResourceManager& other); // Copy Assignment Operator
	ResourceManager& operator = (ResourceManager&& other) noexcept; // Move Assignment Operator

	bool GetModel(ID3D11Device* const device, const char* const modelFileName, ID3D11Buffer* &vertexBuffer, ID3D11Buffer* &indexBuffer);
	bool GetTexture(ID3D11Device* const device, const WCHAR* const textureFileName, ID3D11ShaderResourceView* &texture);

	int GetSizeOfVertexType() const;
	int GetIndexCount(const char* modelFileName) const;

private:
	bool LoadModel(ID3D11Device* const device, const char* const modelFileName);
	bool LoadTexture(ID3D11Device* const device, const WCHAR* textureFileName);

	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	map<const char*, int> m_indexCount;
	map<const char*, int> m_instanceCount;

	map<const char*, ID3D11Buffer*> m_vertexBuffers;
	map<const char*, ID3D11Buffer*> m_indexBuffers;

	map<const WCHAR*, ID3D11ShaderResourceView*> m_textures;
};

