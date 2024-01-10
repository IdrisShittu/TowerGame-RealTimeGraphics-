#include "DepthShader.h"

DepthShader::DepthShader(ID3D11Device* const device, HWND const hwnd) : Shader("DepthVertexShader", "DepthHullShader", "DepthDomainShader", "DepthPixelShader", device, hwnd), m_inputLayout(nullptr)
{
	if (GetInitializationState())
	{
		return;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[7];

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "INSTANCEMATRIX";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[3].InputSlot = 1;
	polygonLayout[3].AlignedByteOffset = 0;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[3].InstanceDataStepRate = 1;

	polygonLayout[4].SemanticName = "INSTANCEMATRIX";
	polygonLayout[4].SemanticIndex = 1;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[4].InputSlot = 1;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[4].InstanceDataStepRate = 1;

	polygonLayout[5].SemanticName = "INSTANCEMATRIX";
	polygonLayout[5].SemanticIndex = 2;
	polygonLayout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[5].InputSlot = 1;
	polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[5].InstanceDataStepRate = 1;

	polygonLayout[6].SemanticName = "INSTANCEMATRIX";
	polygonLayout[6].SemanticIndex = 3;
	polygonLayout[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[6].InputSlot = 1;
	polygonLayout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[6].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[6].InstanceDataStepRate = 1;

	const unsigned int numberOfElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create vertex input layout

	auto result = device->CreateInputLayout(polygonLayout, numberOfElements, GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &m_inputLayout);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}

	//Release buffer resource
	GetVertexShaderBuffer()->Release();
	SetVertexShaderBuffer(nullptr);

	D3D11_SAMPLER_DESC samplerWrapDescription;

	//Create our texture sampler state description
	//Filter determines which pixels will be used/combined for the final look of the texture on the polygon face
	//The filter used is costly in performance but best in visual looks
	samplerWrapDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerWrapDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerWrapDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerWrapDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerWrapDescription.MipLODBias = 0.0f;
	samplerWrapDescription.MaxAnisotropy = 1;
	samplerWrapDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerWrapDescription.BorderColor[0] = 0.0f;
	samplerWrapDescription.BorderColor[1] = 0.0f;
	samplerWrapDescription.BorderColor[2] = 0.0f;
	samplerWrapDescription.BorderColor[3] = 0.0f;
	samplerWrapDescription.MinLOD = 0.0f;
	samplerWrapDescription.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerWrapDescription, &m_sampleStateWrap);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}
}

DepthShader::DepthShader(const DepthShader& other) = default;

DepthShader::DepthShader(DepthShader&& other) noexcept = default;

DepthShader::~DepthShader()
{
	try
	{
		if (m_inputLayout)
		{
			m_inputLayout->Release();
			m_inputLayout = nullptr;
		}
	}
	catch (exception& e)
	{
		
	}
}

DepthShader& DepthShader::operator=(const DepthShader& other) = default;

DepthShader& DepthShader::operator=(DepthShader&& other) noexcept = default;

bool DepthShader::Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
	auto const result = SetDepthShaderParameters(deviceContext, viewMatrix, projectionMatrix, textures, cameraPosition);

	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount, instanceCount);

	return true;
}

bool DepthShader::SetDepthShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const XMFLOAT3& cameraPosition)
{
	const auto result = SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);

	if (!result)
	{
		return false;
	}

	if (textures.size() == 4)
	{
		deviceContext->DSSetShaderResources(0, 1, &textures.back());
	}

	////Need to clear the pixel shader resource as it won't draw to our shader resource view if it's bound to anything
	//ID3D11ShaderResourceView* nullArray[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	//deviceContext->PSSetShaderResources(0, 5, nullArray);

	const auto cameraBuffer = GetCameraBuffer();

	deviceContext->DSSetConstantBuffers(GetDomainBufferResourceCount(), 1, &cameraBuffer);

	IncrementDomainBufferResourceCount();

	return true;
}

void DepthShader::RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const
{
	deviceContext->IASetInputLayout(m_inputLayout);

	SetShader(deviceContext);

	deviceContext->DSSetSamplers(0, 1, &m_sampleStateWrap);

	deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}


