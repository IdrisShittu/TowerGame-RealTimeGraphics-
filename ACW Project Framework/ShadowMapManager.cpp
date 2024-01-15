#include "ShadowMapManager.h"

ShadowMapManager::ShadowMapManager(HWND const hwnd, ID3D11Device* const device, const shared_ptr<Shader>& depthShadr, const int lightCount, const int shadowMapWidth, const int shadowMapHeight) : initializationFailed(false), depthShader(nullptr), renderToTextures(), shadowMapShaderResources()
{
	depthShader = depthShadr;

	//Generate the number of shadow maps depending on the light count
	for (auto i = 0; i < lightCount; i++)
	{
		//Create new render to texture
		renderToTextures.push_back(make_shared<RenderToTexture>(device, shadowMapWidth, shadowMapHeight));

		if (!renderToTextures[i] || renderToTextures[i]->GetInitializationState())
		{
			initializationFailed = true;
			MessageBox(hwnd, "Failed to initialize a render to texture in ShadowMapManager", "Error", MB_OK);
			return;
		}

		renderToTextures[i]->SetShader(depthShader);
	}
}

ShadowMapManager::ShadowMapManager(const ShadowMapManager& other) = default;

ShadowMapManager::ShadowMapManager(ShadowMapManager&& other) noexcept = default;

ShadowMapManager::~ShadowMapManager()
{
}

ShadowMapManager& ShadowMapManager::operator=(const ShadowMapManager& other) = default;

ShadowMapManager& ShadowMapManager::operator=(ShadowMapManager&& other) noexcept = default;

void ShadowMapManager::AddShadowMap(ID3D11Device* const device, const int shadowMapWidth, const int shadowMapHeight)
{
	//Create new render to texture
	renderToTextures.push_back(make_shared<RenderToTexture>(device, shadowMapWidth, shadowMapHeight));

	if (!renderToTextures.back() || renderToTextures.back()->GetInitializationState())
	{
		initializationFailed = true;
		MessageBox(nullptr, "Failed to initialize a render to texture in ShadowMapManager", "Error", MB_OK);
		return;
	}

	renderToTextures.back()->SetShader(depthShader);
}


bool ShadowMapManager::GenerateShadowMapResources(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView, const vector<shared_ptr<Light>>& pointLightList, const vector<shared_ptr<GameObject>>& gameObjects, const XMFLOAT3& cameraPosition)
{
	auto result = true;

	shadowMapShaderResources.clear();

	for (unsigned int i = 0; i < renderToTextures.size(); i++)
	{
		result = renderToTextures[i]->RenderObjectsToTexture(deviceContext, depthStencilView, pointLightList[i]->GetLightViewMatrix(), pointLightList[i]->GetLightProjectionMatrix(),
			pointLightList, gameObjects, cameraPosition);

		if (!result)
		{
			return result;
		}

		shadowMapShaderResources.push_back(renderToTextures[i]->GetShaderResourceView());
	}

	return result;
}

const vector<ID3D11ShaderResourceView*>& ShadowMapManager::GetShadowMapResources() const
{
	return shadowMapShaderResources;
}

bool ShadowMapManager::GetInitializationState() const
{
	return initializationFailed;
}



