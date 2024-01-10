#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Shader.h"
#include "GameObject.h"

using namespace std;
using namespace DirectX;

class RenderToTexture
{
public:
	RenderToTexture(ID3D11Device* const device, const int textureWidth, const int textureHeight);
	RenderToTexture(const RenderToTexture& other); // Copy Constructor
	RenderToTexture(RenderToTexture&& other) noexcept; // Move Constructor
	~RenderToTexture(); 

	RenderToTexture& operator = (const RenderToTexture& other); // Copy Assignment Operator
	RenderToTexture& operator = (RenderToTexture&& other) noexcept; // Move Assignment Operator

	void SetShader(const shared_ptr<Shader>& shader);
	bool RenderObjectsToTexture(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<shared_ptr<Light>>& pointLightList, const vector<shared_ptr<GameObject>>& gameObjects, const XMFLOAT3& cameraPosition) const;

	ID3D11ShaderResourceView* GetShaderResourceView() const;

	bool GetInitializationState() const;

private:
	void SetRenderTarget(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView) const;
	void ClearRenderTarget(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView, const XMFLOAT4& RGBA) const;

	bool m_initializationFailed;

	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;

	shared_ptr<Shader> m_shader;
};

