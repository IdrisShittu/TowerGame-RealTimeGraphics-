#include "ShadowMapManager.h"

ShadowMapManager::ShadowMapManager(HWND const hwnd, ID3D11Device* const device, const shared_ptr<Shader>& depthShader, const int lightCount, const int shadowMapWidth, const int shadowMapHeight) : m_initializationFailed(false), m_depthShader(nullptr), m_renderToTextures(), m_shadowMapShaderResources()
{
	m_depthShader = depthShader;

	//Generate the number of shadow maps depending on the light count
	for (auto i = 0; i < lightCount; i++)
	{
		//Create new render to texture
		m_renderToTextures.push_back(make_shared<RenderToTexture>(device, shadowMapWidth, shadowMapHeight));

		if (!m_renderToTextures[i] || m_renderToTextures[i]->GetInitializationState())
		{
			m_initializationFailed = true;
			MessageBox(hwnd, "Failed to initialize a render to texture in ShadowMapManager", "Error", MB_OK);
			return;
		}

		m_renderToTextures[i]->SetShader(depthShader);
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
	m_renderToTextures.push_back(make_shared<RenderToTexture>(device, shadowMapWidth, shadowMapHeight));

	if (!m_renderToTextures.back() || m_renderToTextures.back()->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(nullptr, "Failed to initialize a render to texture in ShadowMapManager", "Error", MB_OK);
		return;
	}

	m_renderToTextures.back()->SetShader(m_depthShader);
}


bool ShadowMapManager::GenerateShadowMapResources(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView, const vector<shared_ptr<Light>>& pointLightList, const vector<shared_ptr<GameObject>>& gameObjects, const XMFLOAT3& cameraPosition)
{
	auto result = true;

	m_shadowMapShaderResources.clear();

	for (unsigned int i = 0; i < m_renderToTextures.size(); i++)
	{
		result = m_renderToTextures[i]->RenderObjectsToTexture(deviceContext, depthStencilView, pointLightList[i]->GetLightViewMatrix(), pointLightList[i]->GetLightProjectionMatrix(),
			pointLightList, gameObjects, cameraPosition);

		if (!result)
		{
			return result;
		}

		m_shadowMapShaderResources.push_back(m_renderToTextures[i]->GetShaderResourceView());
	}

	return result;
}

const vector<ID3D11ShaderResourceView*>& ShadowMapManager::GetShadowMapResources() const
{
	return m_shadowMapShaderResources;
}

bool ShadowMapManager::GetInitializationState() const
{
	return m_initializationFailed;
}



