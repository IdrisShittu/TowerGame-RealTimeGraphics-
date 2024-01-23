#include "ShaderManager.h"

ShaderManager::ShaderManager(ID3D11Device* const device, HWND const hwnd) : initializationFailed(false), colourShader(nullptr), lightShader(nullptr), reflectionShader(nullptr), texture2DShader(nullptr), textureCubeShader(nullptr), textureNormalShader(nullptr), textureNormalSpecularShader(nullptr)
{
	lightShader = make_shared<LightShader>(device, hwnd);
	lightShader->GetInitializationState();

	reflectionShader = make_shared<ReflectionShader>(device, hwnd);
	reflectionShader->GetInitializationState();
	
	texture2DShader = make_shared<Texture2DShader>(device, hwnd);
	texture2DShader->GetInitializationState();
	textureCubeShader = make_shared<TextureCubeShader>(device, hwnd);

	textureCubeShader->GetInitializationState();
	textureNormalShader = make_shared<TextureNormalMappingShader>(device, hwnd);

	textureNormalShader->GetInitializationState();
	textureNormalSpecularShader = make_shared<TextureNormalSpecularShader>(device, hwnd);
	textureNormalSpecularShader->GetInitializationState();

	textureDisplacementShader = make_shared<TextureDisplacement>(device, hwnd);
	textureDisplacementShader->GetInitializationState();
	depthShader = make_shared<DepthShader>(device, hwnd);
	depthShader->GetInitializationState();
}

ShaderManager::ShaderManager(const ShaderManager& other) = default;

ShaderManager::ShaderManager(ShaderManager&& other) noexcept = default;

ShaderManager::~ShaderManager()
{

}

ShaderManager& ShaderManager::operator=(const ShaderManager& other) = default;

ShaderManager& ShaderManager::operator=(ShaderManager&& other) noexcept = default;

const shared_ptr<Shader>& ShaderManager::GetColourShader() const {
	return colourShader;
}

const shared_ptr<Shader>& ShaderManager::GetLightShader() const {
	return lightShader;
}

const shared_ptr<Shader>& ShaderManager::GetReflectionShader() const
{
	return reflectionShader;
}

const shared_ptr<Shader>& ShaderManager::GetTexture2DShader() const {
	return texture2DShader;
}

const shared_ptr<Shader>& ShaderManager::GetTextureCubeShader() const
{
	return textureCubeShader;
}

const shared_ptr<Shader>& ShaderManager::GetTextureNormalShader() const
{
	return textureNormalShader;
}

const shared_ptr<Shader>& ShaderManager::GetTextureNormalSpecularShader() const
{
	return textureNormalSpecularShader;
}

const shared_ptr<Shader>& ShaderManager::GetTextureDisplacementShader() const
{
	return textureDisplacementShader;
}

const shared_ptr<Shader>& ShaderManager::GetDepthShader() const
{
	return depthShader;
}

bool ShaderManager::GetInitializationState() const
{
	return initializationFailed;
}