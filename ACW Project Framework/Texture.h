#pragma once

//DirectX Libraries
#include <d3d11.h>
#include <d3d11_1.h>

#include "ResourceManager.h"

using namespace DirectX;

class Texture
{
public:
	Texture(ID3D11Device* const device, const vector<const WCHAR*>& textureFileNames, const shared_ptr<ResourceManager>& resourceManager); // Default Constructor
	Texture(const Texture& other); // Copy Constructor
	Texture(Texture&& other) noexcept; // Move Constructor
	~Texture(); // Destructor

	Texture& operator = (const Texture& other); // Copy Assignment Operator
	Texture& operator = (Texture&& other) noexcept; // Move Assignment Operator

	const vector<ID3D11ShaderResourceView*>& GetTextureList() const;

	bool GetInitializationState() const;

private:
	vector<ID3D11ShaderResourceView*> m_texture;

	bool m_initializationFailed;
};