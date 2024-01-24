#include "ParticleShader.h"

ParticleShader::ParticleShader(ID3D11Device* const device, HWND const hwnd) : Shader("ParticleVertexShader", "ParticleHullShader", "ParticleDomainShader", "ParticlePixelShader", device, hwnd), transparency(0.0f), colourTint(0.0f, 0.0f, 0.0f), inputLayout(nullptr), sampleState(nullptr)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout[6];

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

	numberOfElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	auto result = device->CreateInputLayout(polygonLayout, numberOfElements, GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &inputLayout);

	GetVertexShaderBuffer()->Release();
	SetVertexShaderBuffer(nullptr);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}

	D3D11_SAMPLER_DESC samplerDescription;

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

	D3D11_BUFFER_DESC inverseViewMatrixBufferDescription;

	inverseViewMatrixBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	inverseViewMatrixBufferDescription.ByteWidth = sizeof(InverseViewBuffer);
	inverseViewMatrixBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	inverseViewMatrixBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	inverseViewMatrixBufferDescription.MiscFlags = 0;
	inverseViewMatrixBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&inverseViewMatrixBufferDescription, nullptr, &inverseViewMatrixBuffer);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}

	D3D11_BUFFER_DESC particleParametersBufferDescription;

	particleParametersBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	particleParametersBufferDescription.ByteWidth = sizeof(ParticleParametersBuffer);
	particleParametersBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	particleParametersBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	particleParametersBufferDescription.MiscFlags = 0;
	particleParametersBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&particleParametersBufferDescription, nullptr, &particleParametersBuffer);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}
}

ParticleShader::ParticleShader(const ParticleShader& other) = default;

ParticleShader::ParticleShader(ParticleShader&& other) noexcept = default;

ParticleShader::~ParticleShader()
{
	try
	{
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

void ParticleShader::SetParticleParameters(const XMFLOAT3& clTint, const float tr)
{
	colourTint = clTint;
	transparency = tr;
}


bool ParticleShader::Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
	auto const result = SetParticleShaderParameters(deviceContext, viewMatrix, projectionMatrix, textures, cameraPosition);

	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount, instanceCount);

	return true;
}

bool ParticleShader::SetParticleShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const XMFLOAT3& cameraPosition)
{
	const auto result = SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);

	if (!result)
	{
		return false;
	}

	ID3D11ShaderResourceView* textureArray[1];

	copy(textures.begin(), textures.end(), textureArray);
	deviceContext->PSSetShaderResources(0, static_cast<UINT>(textures.size()), textureArray);
	auto mappedResource = GetMappedSubResource();

	auto failed = deviceContext->Map(inverseViewMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(failed))
	{
		return false;
	}

	auto* inverseViewMatrixBufferDataPointer = static_cast<InverseViewBuffer*>(mappedResource.pData);
	const auto inverseViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	inverseViewMatrixBufferDataPointer->inverseViewMatrix = XMMatrixTranspose(inverseViewMatrix);
	inverseViewMatrixBufferDataPointer = nullptr;
	deviceContext->Unmap(inverseViewMatrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(GetVertexBufferResourceCount(), 1, &inverseViewMatrixBuffer);
	deviceContext->DSSetConstantBuffers(GetDomainBufferResourceCount(), 1, &inverseViewMatrixBuffer);
	IncrementVertexBufferResourceCount();
	IncrementDomainBufferResourceCount();
	failed = deviceContext->Map(particleParametersBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(failed))
	{
		return false;
	}

	auto* particleParametersBufferDataPointer = static_cast<ParticleParametersBuffer*>(mappedResource.pData);
	particleParametersBufferDataPointer->colourTint = colourTint;
	particleParametersBufferDataPointer->transparency = transparency;
	particleParametersBufferDataPointer = nullptr;
	deviceContext->Unmap(particleParametersBuffer, 0);
	deviceContext->PSSetConstantBuffers(GetPixelBufferResourceCount(), 1, &particleParametersBuffer);
	IncrementPixelBufferResourceCount();
	return true;
}

void ParticleShader::RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const
{
	deviceContext->IASetInputLayout(inputLayout);
	SetShader(deviceContext);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}
