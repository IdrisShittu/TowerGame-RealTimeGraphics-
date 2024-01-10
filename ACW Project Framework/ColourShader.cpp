#include "ColourShader.h"

ColourShader::ColourShader(ID3D11Device* const device, HWND const hwnd) : Shader("ColourVertexShader", "ColourHullShader", "ColourDomainShader", "ColourPixelShader", device, hwnd), m_inputLayout(nullptr) {
	
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

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
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
	const auto result = device->CreateInputLayout(polygonLayout, numberOfElements, GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &m_inputLayout);

	//Vertex shader buffer isn't need anymore, release it now
	GetVertexShaderBuffer()->Release();
	SetVertexShaderBuffer(nullptr);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}
}

ColourShader::ColourShader(const ColourShader& other) = default;

ColourShader::ColourShader(ColourShader&& other) noexcept = default;

ColourShader::~ColourShader()
{
	try
	{
		//Release resources
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

ColourShader& ColourShader::operator=(const ColourShader& other) = default;

ColourShader& ColourShader::operator=(ColourShader&& other) noexcept = default;

bool ColourShader::Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) {
	
	auto const result = SetColourShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);

	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount, instanceCount);

	return true;
}

bool ColourShader::SetColourShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) {
	
	const auto result = SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);

	return result;
}

void ColourShader::RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const {
	
	//Set input layout
	deviceContext->IASetInputLayout(m_inputLayout);

	//Set our shaders
	SetShader(deviceContext);

	//Render model
	//deviceContext->DrawIndexed(indexCount, 0, 0);
	deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}
