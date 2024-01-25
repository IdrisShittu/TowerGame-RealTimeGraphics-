#include "TextureNormalMappingShader.h"

TextureNormalMappingShader::TextureNormalMappingShader(ID3D11Device* const device, HWND const hwnd) : Shader("TextureNormalVertexShader", "TextureNormalHullShader", "TextureNormalDomainShader", "TextureNormalPixelShader", device, hwnd), inputLayout(nullptr), sampleState(nullptr), lightBuffer(nullptr)
{
	if (GetInitializationState())
	{
		return;
	}

	D3D11_INPUT_ELEMENT_DESC layout[9];

	unsigned int numberOfElements = 0;

	layout[0].SemanticName = "POSITION";
	layout[0].SemanticIndex = 0;
	layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlot = 0;
	layout[0].AlignedByteOffset = 0;
	layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[0].InstanceDataStepRate = 0;

	layout[1].SemanticName = "TEXCOORD";
	layout[1].SemanticIndex = 0;
	layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[1].InputSlot = 0;
	layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;

	layout[2].SemanticName = "NORMAL";
	layout[2].SemanticIndex = 0;
	layout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[2].InputSlot = 0;
	layout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[2].InstanceDataStepRate = 0;

	layout[3].SemanticName = "TANGENT";
	layout[3].SemanticIndex = 0;
	layout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[3].InputSlot = 0;
	layout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[3].InstanceDataStepRate = 0;

	layout[4].SemanticName = "BINORMAL";
	layout[4].SemanticIndex = 0;
	layout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[4].InputSlot = 0;
	layout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[4].InstanceDataStepRate = 0;

	layout[5].SemanticName = "INSTANCEMATRIX";
	layout[5].SemanticIndex = 0;
	layout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[5].InputSlot = 1;
	layout[5].AlignedByteOffset = 0;
	layout[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[5].InstanceDataStepRate = 1;

	layout[6].SemanticName = "INSTANCEMATRIX";
	layout[6].SemanticIndex = 1;
	layout[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[6].InputSlot = 1;
	layout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[6].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[6].InstanceDataStepRate = 1;

	layout[7].SemanticName = "INSTANCEMATRIX";
	layout[7].SemanticIndex = 2;
	layout[7].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[7].InputSlot = 1;
	layout[7].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[7].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[7].InstanceDataStepRate = 1;

	layout[8].SemanticName = "INSTANCEMATRIX";
	layout[8].SemanticIndex = 3;
	layout[8].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[8].InputSlot = 1;
	layout[8].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[8].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[8].InstanceDataStepRate = 1;

	//Get count of elements in layout
	numberOfElements = sizeof(layout) / sizeof(layout[0]);

	//Create vertex input layout
	auto result = device->CreateInputLayout(layout, numberOfElements, GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &inputLayout);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}

	//Release buffer resource
	GetVertexShaderBuffer()->Release();
	SetVertexShaderBuffer(nullptr);

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

	result = device->CreateSamplerState(&samplerDescription, &sampleState);

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

	result = device->CreateBuffer(&lightBufferDescription, nullptr, &lightBuffer);

	if (FAILED(result))
	{
		SetInitializationState(true);
	}
}

TextureNormalMappingShader::TextureNormalMappingShader(const TextureNormalMappingShader& other) = default;

TextureNormalMappingShader::TextureNormalMappingShader(TextureNormalMappingShader&& other) noexcept = default;

TextureNormalMappingShader::~TextureNormalMappingShader()
{
	try
	{
		//Release resources
		if (lightBuffer)
		{
			lightBuffer->Release();
			lightBuffer = nullptr;
		}

		if (sampleState)
		{
			sampleState->Release();
			sampleState = nullptr;
		}

		if (inputLayout)
		{
			inputLayout->Release();
			inputLayout = nullptr;
		}
	}
	catch (exception& e)
	{

	}
}

TextureNormalMappingShader& TextureNormalMappingShader::operator=(const TextureNormalMappingShader& other) = default;

TextureNormalMappingShader& TextureNormalMappingShader::operator=(TextureNormalMappingShader&& other) noexcept = default;

bool TextureNormalMappingShader::Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
	auto const result = SetTextureNormalShaderParameters(deviceContext, viewMatrix, projectionMatrix, textures, pointLightList, cameraPosition);

	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount, instanceCount);

	return true;
}

bool TextureNormalMappingShader::SetTextureNormalShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
	const auto result = SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);

	if (!result)
	{
		return false;
	}

	ID3D11ShaderResourceView* textureArray[2];

	copy(textures.begin(), textures.begin() + 2, textureArray);

	//Set the texture resource to the pixel shader
	deviceContext->PSSetShaderResources(0, 2, textureArray);

	auto mappedResource = GetMappedSubResource();

	//Lock light constant buffer
	const auto failed = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(failed))
	{
		return false;
	}

	auto* lightBufferDataPointer = static_cast<LightBufferType*>(mappedResource.pData);

	//Copy light variables to constant buffer

	for (unsigned int i = 0; i < pointLightList.size(); i++)
	{
		lightBufferDataPointer->lights[i].ambientColour = pointLightList[i]->GetAmbientColour();
		lightBufferDataPointer->lights[i].diffuseColour = pointLightList[i]->GetDiffuseColour();

		const auto lightPos = pointLightList[i]->GetLightPosition();
		lightBufferDataPointer->lights[i].lightPositions = XMFLOAT4(lightPos.x, lightPos.y, lightPos.z, 0.0f);
	}

	lightBufferDataPointer->lightCount = pointLightList.size();
	lightBufferDataPointer->padding = XMFLOAT3();

	lightBufferDataPointer = nullptr;

	//Unlock constant buffer
	deviceContext->Unmap(lightBuffer, 0);

	deviceContext->PSSetConstantBuffers(GetPixelBufferResourceCount(), 1, &lightBuffer);

	IncrementPixelBufferResourceCount();

	return true;
}

void TextureNormalMappingShader::RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const
{
	//Set input layout
	deviceContext->IASetInputLayout(inputLayout);

	//Set our shaders
	SetShader(deviceContext);

	//Set pixel shaders sampler state
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	//Render model
	//deviceContext->DrawIndexed(indexCount, 0, 0);
	deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}
