#pragma once

#include <memory>

//Include all shader header files
#include "ColourShader.h"
#include "LightShader.h"
#include "TextureDisplacement.h"
#include "TextureCubeShader.h"
#include "ReflectionShader.h"
#include "Texture2DShader.h"
#include "TextureNormalMappingShader.h"
#include "TextureNormalSpecularShader.h"
#include "TextureDisplacement.h"
#include "DepthShader.h"

class ShaderManager
{
public:

	ShaderManager(ID3D11Device* const device, HWND const hwnd); //Default Constructor
	ShaderManager(const ShaderManager& other); //Copy Constructor
	ShaderManager(ShaderManager&& other) noexcept; //Move Constructor
	~ShaderManager(); //Destructor

	ShaderManager& operator = (const ShaderManager& other); //Copy Assignment Operator
	ShaderManager& operator = (ShaderManager&& other) noexcept; //Move Assignment Operator

	const shared_ptr<Shader>& GetColourShader() const;
	const shared_ptr<Shader>& GetLightShader() const;
	const shared_ptr<Shader>& GetReflectionShader() const;
	const shared_ptr<Shader>& GetTexture2DShader() const;
	const shared_ptr<Shader>& GetTextureCubeShader() const;
	const shared_ptr<Shader>& GetTextureNormalShader() const;
	const shared_ptr<Shader>& GetTextureNormalSpecularShader() const;
	const shared_ptr<Shader>& GetTextureDisplacementShader() const;
	const shared_ptr<Shader>& GetDepthShader() const;

	//Example methods for current shaders, will need to make one for each new shader created or final ones used
	//bool RenderColourShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	//bool RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColour, XMFLOAT3 lightDirection);
	//bool RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);

	bool GetInitializationState() const;

private:

	bool m_initializationFailed;

	shared_ptr<Shader> m_colourShader;
	shared_ptr<Shader> m_lightShader;
	shared_ptr<Shader> m_reflectionShader;
	shared_ptr<Shader> m_texture2DShader;
	shared_ptr<Shader> m_textureCubeShader;
	shared_ptr<Shader> m_textureNormalShader;
	shared_ptr<Shader> m_textureNormalSpecularShader;
	shared_ptr<Shader> m_textureDisplacementShader;
	shared_ptr<Shader> m_depthShader;
};

