#include "Texture.h"

Texture::Texture(ID3D11Device* const device, const vector<const WCHAR*>& textureFileNames, const shared_ptr<ResourceManager>& resourceManager) : texture(), initializationFailed(false)
{
	for (unsigned int i = 0; i < textureFileNames.size(); i++)
	{
		ID3D11ShaderResourceView* tex = nullptr;

		const auto result = resourceManager->GetTexture(device, textureFileNames[i], tex);

		if (!result)
		{
			initializationFailed = true;
		}

		texture.push_back(tex);

		tex = nullptr;
	}
}

Texture::Texture(const Texture& other) = default;

Texture::Texture(Texture&& other) noexcept = default;

Texture::~Texture()
{
	try
	{
		for (auto& texture : texture)
		{
			if (texture)
			{
				texture = nullptr;
			}
		}
	}
	catch (exception& e)
	{

	}
}

Texture& Texture::operator=(const Texture& other) = default;

Texture& Texture::operator=(Texture&& other) noexcept = default;

const vector<ID3D11ShaderResourceView*>& Texture::GetTextureList() const {
	return texture;
}

bool Texture::GetInitializationState() const {
	return initializationFailed;
}