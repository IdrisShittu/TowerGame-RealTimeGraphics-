#include "Texture2DShader.h"

Texture2DShader::Texture2DShader(ID3D11Device* const device, HWND const hwnd) : Shader("Texture2DVertexShader", "Texture2DHullShader", "Texture2DDomainShader", "Texture2DPixelShader", device, hwnd), m_inputLayout(nullptr), m_sampleState(nullptr)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout[6];

	unsigned int numberOfElements = 0;

	//Setup layout of buffer data in the shader
	//Setup of the layout needs to match the struct in our Model class and the struct in the shader

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

	polygonLayout[2].SemanticName = "INSTANCEMATRIX";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[2].InputSlot = 1;
	polygonLayout[2].AlignedByteOffset = 0;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[2].InstanceDataStepRate = 1;

	polygonLayout[3].SemanticName = "INSTANCEMATRIX";
	polygonLayout[3].SemanticIndex = 1;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[3].InputSlot = 1;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[3].InstanceDataStepRate = 1;

	polygonLayout[4].SemanticName = "INSTANCEMATRIX";
	polygonLayout[4].SemanticIndex = 2;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[4].InputSlot = 1;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[4].InstanceDataStepRate = 1;

	polygonLayout[5].SemanticName = "INSTANCEMATRIX";
	polygonLayout[5].SemanticIndex = 3;
	polygonLayout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[5].InputSlot = 1;
	polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[5].InstanceDataStepRate = 1;

	//Get count of elements in layout
	numberOfElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create vertex input layout
	auto result = device->CreateInputLayout(polygonLayout, numberOfElements, GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &m_inputLayout);

	GetVertexShaderBuffer()->Release();
	SetVertexShaderBuffer(nullptr);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}

	D3D11_SAMPLER_DESC samplerDescription;

	//Create our texture sampler state description
	//Filter determines which pixels will be used/combined for the final look of the texture on the polygon face
	//The filter used is costly in performance but best in visual looks
	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.MipLODBias = 0.0f;
	samplerDescription.MaxAnisotropy = 1;
	samplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDescription.BorderColor[0] = 0.0f;
	samplerDescription.BorderColor[1] = 0.0f;
	samplerDescription.BorderColor[2] = 0.0f;
	samplerDescription.BorderColor[3] = 0.0f;
	samplerDescription.MinLOD = 0.0f;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDescription, &m_sampleState);

	if (FAILED(result))
	{
		SetInitializationState(true);
	}
}

Texture2DShader::Texture2DShader(const Texture2DShader& other) = default;

Texture2DShader::Texture2DShader(Texture2DShader&& other) noexcept = default;

Texture2DShader::~Texture2DShader()
{
	try
	{
		//Release resources
		if (m_sampleState)
		{
			m_sampleState->Release();
			m_sampleState = nullptr;
		}

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

Texture2DShader& Texture2DShader::operator=(const Texture2DShader& other) = default;

Texture2DShader& Texture2DShader::operator=(Texture2DShader&& other) noexcept = default;

bool Texture2DShader::Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) {
	
	auto const result = SetTextureShaderParameters(deviceContext, viewMatrix, projectionMatrix, textures, cameraPosition);

	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount, instanceCount);

	return true;
}

bool Texture2DShader::SetTextureShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const XMFLOAT3& cameraPosition) {
	
	const auto result = SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);

	if (!result)
	{
		return false;
	}

	//Hopefully this works, doubt there is going to ever be more than 10 textures
	ID3D11ShaderResourceView* textureArray[1];

	copy(textures.begin(), textures.begin() + 1, textureArray);

	//Set the texture resource to the pixel shader
	deviceContext->PSSetShaderResources(0, 1, textureArray);

	return true;
}

void Texture2DShader::RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const {
	//Set input layout
	deviceContext->IASetInputLayout(m_inputLayout);

	//Set our shaders
	SetShader(deviceContext);

	//Set pixel shaders sampler state
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//Render triangle
	//deviceContext->DrawIndexed(indexCount, 0, 0);
	deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}