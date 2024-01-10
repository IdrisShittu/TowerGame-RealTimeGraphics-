#include "RenderToTexture.h"

RenderToTexture::RenderToTexture(ID3D11Device* const device, const int textureWidth, const int textureHeight) : m_initializationFailed(false), m_renderTargetTexture(nullptr), m_renderTargetView(nullptr), m_shaderResourceView(nullptr), m_shader(nullptr)
{
	//Create texture description
	D3D11_TEXTURE2D_DESC textureDescription;

	ZeroMemory(&textureDescription, sizeof(textureDescription));

	textureDescription.Width = textureWidth;
	textureDescription.Height = textureHeight;
	textureDescription.MipLevels = 1;
	textureDescription.ArraySize = 1;
	textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDescription.SampleDesc.Count = 1;
	textureDescription.SampleDesc.Quality = 0;
	textureDescription.Usage = D3D11_USAGE_DEFAULT;
	textureDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDescription.CPUAccessFlags = 0;
	textureDescription.MiscFlags = 0;

	//Create render target texture
	auto result = device->CreateTexture2D(&textureDescription, nullptr, &m_renderTargetTexture);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Create render target view description
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescription;

	renderTargetViewDescription.Format = textureDescription.Format;
	renderTargetViewDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDescription.Texture2D.MipSlice = 0;

	//Create render target view
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDescription, &m_renderTargetView);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Create shader resource view description
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription;

	shaderResourceViewDescription.Format = textureDescription.Format;
	shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDescription.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDescription.Texture2D.MipLevels = 1;

	//Create shader resource view
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDescription, &m_shaderResourceView);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}
}

RenderToTexture::RenderToTexture(const RenderToTexture& other) = default;

RenderToTexture::RenderToTexture(RenderToTexture&& other) noexcept = default;

RenderToTexture::~RenderToTexture()
{
	try
	{
		if (m_shaderResourceView)
		{
			m_shaderResourceView->Release();
			m_shaderResourceView = nullptr;
		}

		if (m_renderTargetView)
		{
			m_renderTargetView->Release();
			m_renderTargetView = nullptr;
		}

		if (m_renderTargetTexture)
		{
			m_renderTargetTexture->Release();
			m_renderTargetTexture = nullptr;
		}
	}
	catch (exception& e)
	{

	}
}

RenderToTexture& RenderToTexture::operator=(const RenderToTexture& other) = default;

RenderToTexture& RenderToTexture::operator=(RenderToTexture&& other) noexcept = default;

void RenderToTexture::SetShader(const shared_ptr<Shader>& shader)
{
	m_shader = shader;
}

bool RenderToTexture::RenderObjectsToTexture(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<shared_ptr<Light>>& pointLightList, const vector<shared_ptr<GameObject>>& gameObjects, const XMFLOAT3& cameraPosition) const
{
	SetRenderTarget(deviceContext, depthStencilView);

	ClearRenderTarget(deviceContext, depthStencilView, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	////Swap viewports to texture sized one
	//UINT numberOfViewports = 1;
	//D3D11_VIEWPORT viewport;

	//deviceContext->RSGetViewports(&numberOfViewports, &viewport);

	//deviceContext->RSSetViewports(1, &m_viewport);

	//For each gameobject we want to swap the shader to the shader given and then swap it back
	for (const auto& gameObject : gameObjects)
	{
		const auto originalShader = gameObject->GetShaderComponent();

		if (m_shader)
		{
			auto mipInterval = 0.0f;
			auto mipClampMinimum = 0.0f;
			auto mipClampMaximum = 0.0f;
			auto displacementPower = 0.0f;

			gameObject->SetShaderComponent(m_shader);

			originalShader->GetDisplacementVariables(mipInterval, mipClampMinimum, mipClampMaximum, displacementPower);
			m_shader->SetDisplacementVariables(mipInterval, mipClampMinimum, mipClampMaximum, displacementPower);
		}

		const auto result = gameObject->Render(deviceContext, viewMatrix, projectionMatrix, {}, pointLightList, cameraPosition);

		if (m_shader)
		{
			//Set shader back to the original shader
			gameObject->SetShaderComponent(originalShader);
		}

		if (!result)
		{
			return false;
		}
	}

	//deviceContext->RSSetViewports(1, &viewport);

	return true;
}

void RenderToTexture::SetRenderTarget(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView) const {

	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);
}

void RenderToTexture::ClearRenderTarget(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView, const XMFLOAT4& RGBA) const
{
	const float colour[4] {RGBA.x, RGBA.y, RGBA.z, RGBA.w};
	
	deviceContext->ClearRenderTargetView(m_renderTargetView, colour);

	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderToTexture::GetShaderResourceView() const {
	return m_shaderResourceView;
}

bool RenderToTexture::GetInitializationState() const {
	return m_initializationFailed;
}
