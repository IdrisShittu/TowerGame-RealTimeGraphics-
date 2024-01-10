#pragma once

//Link DirectX libraries we are using to this object
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "d3d11.lib")

//DirectX libraries
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

#include <Windows.h>

using namespace DirectX;
using namespace std;

class D3DContainer
{
public:
	D3DContainer(int const screenWidth, int const screenHeight, HWND const hwnd, bool const fullScreen, bool const vSyncEnabled, float const screenDepth, float const screenNear); // Default Constructor
	D3DContainer(const D3DContainer& other); // Copy Constructor
	D3DContainer(D3DContainer && other) noexcept; // Move Constructor
	~D3DContainer(); // Destructor

	D3DContainer& operator = (const D3DContainer& other); // Copy Assignment Operator
	//D3DContainer& operator = (D3DContainer&& other) noexcept; // Move Assignment Operator

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;

	ID3D11DepthStencilView* GetDepthStencilView() const;
	void SetRenderTarget() const;

	void EnableWireFrame() const;
	void DisableWireFrame() const;

	void EnabledDepthStencil() const;
	void DisableDepthStencil() const;

	void EnableAlphaBlending() const;
	void DisableAlphaBlending() const;

	void GetProjectionMatrix(XMMATRIX& projectionMatrix) const;
	void GetOrthogonalMatrix(XMMATRIX& orthographicMatrix) const;

	void GetVideoCardInfo(char* const cardName, int& memory) const;

	bool GetInitializationState() const;

private:

	void InitializeFactoryAdapter(unsigned int const screenWidth, unsigned int const screenHeight, unsigned int &numerator, unsigned int &denominator);
	void InitializeDeviceAndSwapChain(unsigned int const screenWidth, unsigned int const screenHeight, unsigned int const numerator, unsigned int const denominator, bool const fullScreen, HWND const hwnd);
	void InitializeBuffers(unsigned int const screenWidth, unsigned int const screenHeight);

	bool m_initializationFailed;

	bool m_vSyncEnabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;

	ID3D11RenderTargetView* m_renderTargetView;

	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilStateEnabled;
	ID3D11DepthStencilState* m_depthStencilStateDisabled;;
	ID3D11DepthStencilView* m_depthStencilView;

	ID3D11RasterizerState* m_rasterStateNormal;
	ID3D11RasterizerState* m_rasterStateWireFrame;

	ID3D11BlendState* m_alphaEnabledBlendState;
	ID3D11BlendState* m_alphaDisableBlendState;

	XMFLOAT4X4 m_projectionMatrix;
	XMFLOAT4X4 m_orthographicMatrix;
};
