#include "ShaderManager.h"

//So we only need one instance of each shader and the gameobject just declares the shader choice

ShaderManager::ShaderManager(ID3D11Device* const device, HWND const hwnd) : initializationFailed(false), colourShader(nullptr), lightShader(nullptr), reflectionShader(nullptr), texture2DShader(nullptr), textureCubeShader(nullptr), textureNormalShader(nullptr), textureNormalSpecularShader(nullptr)
{
	//Initialize our shaders
	/*colourShader = new ColourShader(device, hwnd);

	if (colourShader->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the colour shader", "Error", MB_OK);
		return;
	}*/

	lightShader = make_shared<LightShader>(device, hwnd);

	if (lightShader->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the light shader", "Error", MB_OK);
		return;
	}

	reflectionShader = make_shared<ReflectionShader>(device, hwnd);

	if (reflectionShader->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the reflection shader", "Error", MB_OK);
		return;
	}

	texture2DShader = make_shared<Texture2DShader>(device, hwnd);

	if (texture2DShader->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the texture 2D shader", "Error", MB_OK);
		return;
	}

	textureCubeShader = make_shared<TextureCubeShader>(device, hwnd);

	if (textureCubeShader->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(hwnd, "Colour not initialize the texture cube shader", "Error", MB_OK);
		return;
	}

	textureNormalShader = make_shared<TextureNormalMappingShader>(device, hwnd);

	if (textureNormalShader->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the texture normal shader", "Error", MB_OK);
		return;
	}

	textureNormalSpecularShader = make_shared<TextureNormalSpecularShader>(device, hwnd);

	if (textureNormalSpecularShader->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the texture normal specular shader", "Error", MB_OK);
		return;
	}

	textureDisplacementShader = make_shared<TextureDisplacement>(device, hwnd);

	if (textureDisplacementShader->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the texture displacement shader", "Error", MB_OK);
		return;
	}

	depthShader = make_shared<DepthShader>(device, hwnd);

	if (depthShader->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the depth shader", "Error", MB_OK);
		return;
	}
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