#include "Texture.h"

Texture::Texture(ID3D11Device* const device, const vector<const WCHAR*>& textureFileNames, const shared_ptr<ResourceManager>& resourceManager) : m_texture(), m_initializationFailed(false)
{
	for (unsigned int i = 0; i < textureFileNames.size(); i++)
	{
		ID3D11ShaderResourceView* texture = nullptr;

		const auto result = resourceManager->GetTexture(device, textureFileNames[i], texture);

		if (!result)
		{
			m_initializationFailed = true;
		}

		m_texture.push_back(texture);

		texture = nullptr;
	}
}

Texture::Texture(const Texture& other) = default;

Texture::Texture(Texture&& other) noexcept = default;

Texture::~Texture()
{
	try
	{
		for (auto& texture : m_texture)
		{
			if (texture)
			{
				//Don't release, resource manager does this
				//texture->Release();
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
	return m_texture;
}

bool Texture::GetInitializationState() const {
	return m_initializationFailed;
}