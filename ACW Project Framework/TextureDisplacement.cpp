#include "TextureDisplacement.h"

TextureDisplacement::TextureDisplacement(ID3D11Device* const device, HWND const hwnd) : Shader("TextureDisplacementVS", "TextureDisplacementHS", "TextureDisplacementDS", "TextureDisplacementPS", device, hwnd), m_inputLayout(nullptr), m_sampleStateWrap(nullptr), m_sampleStateClamp(nullptr), m_lightBuffer(nullptr)
{
	if (GetInitializationState())
	{
		return;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[9];

	unsigned int numberOfElements = 0;

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

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	polygonLayout[5].SemanticName = "INSTANCEMATRIX";
	polygonLayout[5].SemanticIndex = 0;
	polygonLayout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[5].InputSlot = 1;
	polygonLayout[5].AlignedByteOffset = 0;
	polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[5].InstanceDataStepRate = 1;

	polygonLayout[6].SemanticName = "INSTANCEMATRIX";
	polygonLayout[6].SemanticIndex = 1;
	polygonLayout[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[6].InputSlot = 1;
	polygonLayout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[6].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[6].InstanceDataStepRate = 1;

	polygonLayout[7].SemanticName = "INSTANCEMATRIX";
	polygonLayout[7].SemanticIndex = 2;
	polygonLayout[7].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[7].InputSlot = 1;
	polygonLayout[7].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[7].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[7].InstanceDataStepRate = 1;

	polygonLayout[8].SemanticName = "INSTANCEMATRIX";
	polygonLayout[8].SemanticIndex = 3;
	polygonLayout[8].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[8].InputSlot = 1;
	polygonLayout[8].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[8].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[8].InstanceDataStepRate = 1;

	//Get count of elements in layout
	numberOfElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

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

	D3D11_SAMPLER_DESC samplerClampDescription;

	samplerClampDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerClampDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerClampDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerClampDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerClampDescription.MipLODBias = 0.0f;
	samplerClampDescription.MaxAnisotropy = 1;
	samplerClampDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerClampDescription.BorderColor[0] = 0.0f;
	samplerClampDescription.BorderColor[1] = 0.0f;
	samplerClampDescription.BorderColor[2] = 0.0f;
	samplerClampDescription.BorderColor[3] = 0.0f;
	samplerClampDescription.MinLOD = 0.0f;
	samplerClampDescription.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerClampDescription, &m_sampleStateClamp);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}

	D3D11_BUFFER_DESC lightMatrixBufferDescription;

	lightMatrixBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	lightMatrixBufferDescription.ByteWidth = sizeof(LightMatrixBufferType);
	lightMatrixBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightMatrixBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightMatrixBufferDescription.MiscFlags = 0;
	lightMatrixBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&lightMatrixBufferDescription, nullptr, &m_lightMatrixBuffer);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}

	D3D11_BUFFER_DESC lightBufferDescription;

	lightBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDescription.ByteWidth = sizeof(LightBufferType); // Is a multiple of 16 because our extra float is inside
	lightBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDescription.MiscFlags = 0;
	lightBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDescription, nullptr, &m_lightBuffer);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}
}

TextureDisplacement::TextureDisplacement(const TextureDisplacement& other) = default;

TextureDisplacement::TextureDisplacement(TextureDisplacement&& other) noexcept = default;

TextureDisplacement::~TextureDisplacement()
{
	try
	{
		if (m_lightBuffer)
		{
			m_lightBuffer->Release();
			m_lightBuffer = nullptr;
		}

		if(m_lightMatrixBuffer)
		{
			m_lightMatrixBuffer->Release();
			m_lightMatrixBuffer = nullptr;
		}

		if (m_sampleStateClamp)
		{
			m_sampleStateClamp->Release();
			m_sampleStateClamp = nullptr;
		}

		if (m_sampleStateWrap)
		{
			m_sampleStateWrap->Release();
			m_sampleStateWrap = nullptr;
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

TextureDisplacement& TextureDisplacement::operator=(const TextureDisplacement& other) = default;

TextureDisplacement& TextureDisplacement::operator=(TextureDisplacement&& other) noexcept = default;

bool TextureDisplacement::Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
	auto const result = SetTextureDisplacementShaderParameters(deviceContext, viewMatrix, projectionMatrix, textures, depthTextures, pointLightList, cameraPosition);

	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount, instanceCount);

	return true;
}

bool TextureDisplacement::SetTextureDisplacementShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
	const auto result = SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);

	if (!result)
	{
		return false;
	}

	deviceContext->DSSetShaderResources(0, 1, &textures.back());

	//Set the texture resource array to the pixel shader
	ID3D11ShaderResourceView* pixelShaderTextureArray[3];

	copy(textures.begin(), textures.end() - 1, pixelShaderTextureArray);

	deviceContext->PSSetShaderResources(0, static_cast<UINT>(textures.size()) - 1, pixelShaderTextureArray);

	//Set the texture resource array to the pixel shader
	ID3D11ShaderResourceView* depthTextureArray[MAX_LIGHTS];

	copy(depthTextures.begin(), depthTextures.end(), depthTextureArray);

	deviceContext->PSSetShaderResources(3, static_cast<UINT>(depthTextures.size()), depthTextureArray);
	
	auto mappedResource = GetMappedSubResource();

	//Lock light constant buffer
	auto failed = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(failed))
	{
		return false;
	}

	auto* lightBufferDataPointer = static_cast<LightBufferType*>(mappedResource.pData);

	//Populate array with positions
	for (unsigned int i = 0; i < pointLightList.size(); i++)
	{
		lightBufferDataPointer->lights[i].ambientColour = pointLightList[i]->GetAmbientColour();
		lightBufferDataPointer->lights[i].diffuseColour = pointLightList[i]->GetDiffuseColour();
		lightBufferDataPointer->lights[i].specularColour = pointLightList[i]->GetSpecularColour();
		lightBufferDataPointer->lights[i].lightPositions = pointLightList[i]->GetLightPosition();
		lightBufferDataPointer->lights[i].specularPower = pointLightList[i]->GetSpecularPower();
		lightBufferDataPointer->lights[i].isDirectionalLight = pointLightList[i]->GetIsDirectionalLight();
		lightBufferDataPointer->lights[i].padding = XMFLOAT3();
	}

	lightBufferDataPointer->lightCount = pointLightList.size();
	lightBufferDataPointer->padding = XMFLOAT3();

	lightBufferDataPointer = nullptr;

	//Unlock constant buffer
	deviceContext->Unmap(m_lightBuffer, 0);

	//Set light constant buffer in the pixel shader
	deviceContext->PSSetConstantBuffers(GetPixelBufferResourceCount(), 1, &m_lightBuffer);

	IncrementPixelBufferResourceCount();

	//Lock light constant buffer
	failed = deviceContext->Map(m_lightMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(failed))
	{
		return false;
	}

	auto* lightMatrixBufferDataPointer = static_cast<LightMatrixBufferType*>(mappedResource.pData);

	for (unsigned int i = 0; i < pointLightList.size(); i++)
	{
		lightMatrixBufferDataPointer->lights[i].lightViewMatrix = XMMatrixTranspose(pointLightList[i]->GetLightViewMatrix());
		lightMatrixBufferDataPointer->lights[i].lightProjectionMatrix = XMMatrixTranspose(pointLightList[i]->GetLightProjectionMatrix());
	}

	lightMatrixBufferDataPointer->lightCount = pointLightList.size();
	lightMatrixBufferDataPointer->padding = XMFLOAT3();

	lightMatrixBufferDataPointer = nullptr;

	//Unlock constant buffer
	deviceContext->Unmap(m_lightMatrixBuffer, 0);

	//Set light matrix constant buffer to domain shader
	deviceContext->DSSetConstantBuffers(GetDomainBufferResourceCount(), 1, &m_lightMatrixBuffer);

	IncrementDomainBufferResourceCount();

	const auto cameraBuffer = GetCameraBuffer();

	deviceContext->DSSetConstantBuffers(GetDomainBufferResourceCount(), 1, &cameraBuffer);

	IncrementDomainBufferResourceCount();

	return true;
}

void TextureDisplacement::RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const
{
	//Set input layout
	deviceContext->IASetInputLayout(m_inputLayout);

	SetShader(deviceContext);

	//Set pixel and domain shaders sampler state
	deviceContext->DSSetSamplers(0, 1, &m_sampleStateWrap);
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateWrap);
	deviceContext->PSSetSamplers(1, 1, &m_sampleStateClamp);

	deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
	//deviceContext->DrawIndexed(indexCount, 0, 0);
}
