#include "Shader.h"

Shader::Shader(const string& vertexShaderFileName, const string& hullShaderFileName, const string& domainShaderFileName, const string& pixelShaderFileName, ID3D11Device* const device, HWND const hwnd) : m_initializationFailed(false), m_vertexBufferResourceCount(0), m_hullBufferResourceCount(0), m_domainBufferResourceCount(0), m_pixelBufferResourceCount(0), m_nonTextureRenderMode(0), m_textureDiffuseRenderMode(0), m_displacementRenderMode(0), m_maxTessellationDistance(1.0f), m_minTessellationDistance(1.0f), m_maxTessellationFactor(0.0f), m_minTessellationFactor(0.0f), m_mipInterval(0.0f), m_mipClampMinimum(0.0f), m_mipClampMaximum(0.0f), m_displacementPower(0.0f), m_vertexShaderBuffer(nullptr), m_vertexShader(nullptr), m_hullShader(nullptr), m_domainShader(nullptr), m_pixelShader(nullptr), m_matrixBuffer(nullptr), m_tessellationBuffer(nullptr), m_cameraBuffer(nullptr), m_renderModeBuffer(nullptr)
{
	ID3D10Blob* errorMessage = nullptr;

	const unsigned int numberOfElements = 0;

	//Load our Vertex and Pixel Shader

	const auto hlslVertexFileName = vertexShaderFileName + ".hlsl";

	auto result = D3DCompileFromFile(CA2W(hlslVertexFileName.c_str()), nullptr, nullptr, vertexShaderFileName.c_str(), "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, /*D3DCOMPILE_DEBUG*/ 0, &m_vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		m_initializationFailed = true;

		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, hlslVertexFileName.c_str());
		}
		else
		{
			MessageBox(hwnd, hlslVertexFileName.c_str(), "Missing Vertex Shader File", MB_OK);
		}

		return;
	}

	const auto hlslHullFileName = hullShaderFileName + ".hlsl";

	ID3D10Blob* hullShaderBuffer = nullptr;

	 result = D3DCompileFromFile(CA2W(hlslHullFileName.c_str()), nullptr, nullptr, hullShaderFileName.c_str(), "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &hullShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		SetInitializationState(true);

		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, hlslHullFileName.c_str());
		}
		else
		{
			MessageBox(hwnd, hlslHullFileName.c_str(), "Missing Hull Shader File", MB_OK);
		}

		return;
	}

	const auto hlslDomainFileName = domainShaderFileName + ".hlsl";

	ID3D10Blob* domainShaderBuffer = nullptr;

	result = D3DCompileFromFile(CA2W(hlslDomainFileName.c_str()), nullptr, nullptr, domainShaderFileName.c_str(), "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &domainShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		SetInitializationState(true);

		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, hlslHullFileName.c_str());
		}
		else
		{
			MessageBox(hwnd, hlslHullFileName.c_str(), "Missing Domain Shader File", MB_OK);
		}

		return;
	}

	const auto hlslPixelFileName = pixelShaderFileName + ".hlsl";

	ID3D10Blob* pixelShaderBuffer = nullptr;

	result = D3DCompileFromFile(CA2W(hlslPixelFileName.c_str()), nullptr, nullptr, pixelShaderFileName.c_str(), "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		m_initializationFailed = true;

		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, hlslPixelFileName.c_str());
		}
		else
		{
			MessageBox(hwnd, hlslPixelFileName.c_str(), "Missing Pixel Shader File", MB_OK);
		}

		return;
	}

	//Create vertex, hull, domain and pixel shader from buffer
	result = device->CreateVertexShader(m_vertexShaderBuffer->GetBufferPointer(), m_vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), nullptr, &m_hullShader);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), nullptr, &m_domainShader);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Release shader buffer resources as we don't need them anymore
	hullShaderBuffer->Release();
	hullShaderBuffer = nullptr;

	domainShaderBuffer->Release();
	domainShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	D3D11_BUFFER_DESC matrixBufferDescription;

	//Setup the description of the dynamic matrix constant buffer that is in the domain shader
	matrixBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDescription.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDescription.MiscFlags = 0;
	matrixBufferDescription.StructureByteStride = 0;

	//Create constant buffer pointer so we can access the vertex shaders constant buffer
	result = device->CreateBuffer(&matrixBufferDescription, nullptr, &m_matrixBuffer);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	D3D11_BUFFER_DESC tessellationBufferDescription;

	tessellationBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDescription.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDescription.MiscFlags = 0;
	tessellationBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&tessellationBufferDescription, nullptr, &m_tessellationBuffer);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	D3D11_BUFFER_DESC cameraBufferDescription;

	cameraBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDescription.ByteWidth = sizeof(CameraBufferType); // Is a multiple of 16 because our extra float is inside
	cameraBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDescription.MiscFlags = 0;
	cameraBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&cameraBufferDescription, nullptr, &m_cameraBuffer);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	D3D11_BUFFER_DESC displacementBufferDescription;

	displacementBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	displacementBufferDescription.ByteWidth = sizeof(DisplacementBuffer);
	displacementBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	displacementBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	displacementBufferDescription.MiscFlags = 0;
	displacementBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&displacementBufferDescription, nullptr, &m_displacementBuffer);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	D3D11_BUFFER_DESC renderModeBufferDescription;

	renderModeBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	renderModeBufferDescription.ByteWidth = sizeof(RenderModeBufferType);
	renderModeBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	renderModeBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	renderModeBufferDescription.MiscFlags = 0;
	renderModeBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&renderModeBufferDescription, nullptr, &m_renderModeBuffer);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}
}

Shader::Shader(const Shader& other) = default;

Shader::Shader(Shader&& other) noexcept = default;

Shader::~Shader() {

	try
	{
		if (m_renderModeBuffer)
		{
			m_renderModeBuffer->Release();
			m_renderModeBuffer = nullptr;
		}

		if (m_cameraBuffer)
		{
			m_cameraBuffer->Release();
			m_cameraBuffer = nullptr;
		}

		if (m_tessellationBuffer)
		{
			m_tessellationBuffer->Release();
			m_tessellationBuffer = nullptr;
		}

		if (m_matrixBuffer)
		{
			m_matrixBuffer->Release();
			m_matrixBuffer = nullptr;
		}

		if (m_pixelShader)
		{
			m_pixelShader->Release();
			m_pixelShader = nullptr;
		}

		if (m_domainShader)
		{
			m_domainShader->Release();
			m_domainShader = nullptr;
		}

		if (m_hullShader)
		{
			m_hullShader->Release();
			m_hullShader = nullptr;
		}

		if (m_vertexShader)
		{
			m_vertexShader->Release();
			m_vertexShader = nullptr;
		}

		if (m_vertexShaderBuffer)
		{
			m_vertexShaderBuffer->Release();
			m_vertexShaderBuffer = nullptr;
		}
	}
	catch (exception& e)
	{
		
	}
}

Shader& Shader::operator=(const Shader& other) = default;

Shader& Shader::operator=(Shader&& other) noexcept = default;


bool Shader::GetInitializationState() const {
	return m_initializationFailed;
}

int Shader::GetVertexBufferResourceCount() const {
	return m_vertexBufferResourceCount;
}

int Shader::GetHullBufferResourceCount() const {
	return m_hullBufferResourceCount;
}

int Shader::GetDomainBufferResourceCount() const {
	return m_domainBufferResourceCount;
}

int Shader::GetPixelBufferResourceCount() const {
	return m_pixelBufferResourceCount;
}

void Shader::IncrementVertexBufferResourceCount() {
	m_vertexBufferResourceCount++;
}

void Shader::IncrementHullBufferResourceCount() {
	m_hullBufferResourceCount++;
}

void Shader::IncrementDomainBufferResourceCount() {
	m_domainBufferResourceCount++;
}

void Shader::IncrementPixelBufferResourceCount() {
	m_pixelBufferResourceCount++;
}

ID3D10Blob* Shader::GetVertexShaderBuffer() const
{
	return m_vertexShaderBuffer;
}

ID3D11Buffer* Shader::GetMatrixBuffer() const
{
	return m_matrixBuffer;
}

ID3D11Buffer* Shader::GetCameraBuffer() const {
	return m_cameraBuffer;
}

const D3D11_MAPPED_SUBRESOURCE& Shader::GetMappedSubResource() const
{
	return m_mappedResource;
}

void Shader::SetRenderModeStates(const int nonTextured, const int texturedDiffuse, const int displacementEnabled)
{
	m_nonTextureRenderMode = nonTextured;
	m_textureDiffuseRenderMode = texturedDiffuse;
	m_displacementRenderMode = displacementEnabled;
}

void Shader::GetTessellationVariables(float& maxTessellationDistance, float& minTessellationDistance, float& maxTessellationFactor, float& minTessellationFactor) const
{
	maxTessellationDistance = m_maxTessellationDistance;
	minTessellationDistance = m_minTessellationDistance;
	maxTessellationFactor = m_maxTessellationFactor;
	minTessellationFactor = m_minTessellationFactor;
}

void Shader::SetTessellationVariables(const float& maxTessellationDistance, const float& minTessellationDistance, const float& maxTessellationFactor, const float& minTessellationFactor)
{
	m_maxTessellationDistance = maxTessellationDistance;
	m_minTessellationDistance = minTessellationDistance;
	m_maxTessellationFactor = maxTessellationFactor;
	m_minTessellationFactor = minTessellationFactor;
}

void Shader::GetDisplacementVariables(float& mipInterval, float& mipClampMinimum, float& mipClampMaximum, float& displacementPower) const
{
	mipInterval = m_mipInterval;
	mipClampMinimum = m_mipClampMinimum;
	mipClampMaximum = m_mipClampMaximum;
	displacementPower = m_displacementPower;
}

void Shader::SetDisplacementVariables(const float& mipInterval, const float& mipClampMinimum, const float& mipClampMaximum, const float& displacementPower)
{
	m_mipInterval = mipInterval;
	m_mipClampMinimum = mipClampMinimum;
	m_mipClampMaximum = mipClampMaximum;
	m_displacementPower = displacementPower;
}

void Shader::SetInitializationState(const bool state)
{
	m_initializationFailed = state;
}

void Shader::SetVertexShaderBuffer(ID3D10Blob* const vertexShaderBuffer)
{
	m_vertexShaderBuffer = vertexShaderBuffer;
}


bool Shader::SetShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) {
	
	m_vertexBufferResourceCount = 0;
	m_hullBufferResourceCount = 0;
	m_domainBufferResourceCount = 0;
	m_pixelBufferResourceCount = 0;

	//Lock matrix constant buffer and set the transposed matrices to it
	auto result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* matrixVertexBufferDataPointer = static_cast<MatrixBufferType*>(m_mappedResource.pData);

	//This will be our world matrix inverse
	matrixVertexBufferDataPointer->viewMatrix = XMMatrixTranspose(XMMATRIX());
	//This will be our texture transform scale matrix
	matrixVertexBufferDataPointer->projectionMatrix = XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 0.0f));

	matrixVertexBufferDataPointer = nullptr;

	//Unlock constant buffer
	deviceContext->Unmap(m_matrixBuffer, 0);

	//Set the updated constant buffer
	deviceContext->VSSetConstantBuffers(m_vertexBufferResourceCount, 1, &m_matrixBuffer);

	m_vertexBufferResourceCount++;

	//Lock matrix constant buffer and set the transposed matrices to it
	result = deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* tessellationBufferDataPointer = static_cast<TessellationBufferType*>(m_mappedResource.pData);


	tessellationBufferDataPointer->maxTessellationDistance = m_maxTessellationDistance;
	tessellationBufferDataPointer->minTessellationDistance = m_minTessellationDistance;
	tessellationBufferDataPointer->maxTessellationFactor = m_maxTessellationFactor;
	tessellationBufferDataPointer->minTessellationFactor = m_minTessellationFactor;

	tessellationBufferDataPointer = nullptr;

	//Unlock constant buffer
	deviceContext->Unmap(m_tessellationBuffer, 0);

	//Set the updated constant buffer
	deviceContext->VSSetConstantBuffers(m_vertexBufferResourceCount, 1, &m_tessellationBuffer);

	m_vertexBufferResourceCount++;

	//Lock camera constant buffer
	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* cameraBufferDataPointer = static_cast<CameraBufferType*>(m_mappedResource.pData);

	cameraBufferDataPointer->cameraPosition = cameraPosition;
	cameraBufferDataPointer->padding = 0.0f;

	cameraBufferDataPointer = nullptr;

	//Unlock constant buffer
	deviceContext->Unmap(m_cameraBuffer, 0);

	//Set camera constant buffer in the vertex shader and DomainShader
	deviceContext->VSSetConstantBuffers(m_vertexBufferResourceCount, 1, &m_cameraBuffer);

	m_vertexBufferResourceCount++;

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* matrixDomainBufferDataPointer = static_cast<MatrixBufferType*>(m_mappedResource.pData);

	matrixDomainBufferDataPointer->viewMatrix = XMMatrixTranspose(viewMatrix);
	matrixDomainBufferDataPointer->projectionMatrix = XMMatrixTranspose(projectionMatrix);

	matrixDomainBufferDataPointer = nullptr;

	deviceContext->Unmap(m_matrixBuffer, 0);

	deviceContext->DSSetConstantBuffers(m_domainBufferResourceCount, 1, &m_matrixBuffer);

	m_domainBufferResourceCount++;

	//Lock matrix constant buffer and set the transposed matrices to it
	result = deviceContext->Map(m_displacementBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* displacementBufferDataPointer = static_cast<DisplacementBuffer*>(m_mappedResource.pData);

	displacementBufferDataPointer->mipInterval = m_mipInterval;
	displacementBufferDataPointer->mipMinimum = m_mipClampMinimum;
	displacementBufferDataPointer->mipMaximum = m_mipClampMaximum;
	displacementBufferDataPointer->displacementPower = m_displacementPower;
	displacementBufferDataPointer->displacementEnabled = m_displacementRenderMode;
	displacementBufferDataPointer->padding = XMFLOAT3();

	displacementBufferDataPointer = nullptr;

	deviceContext->Unmap(m_displacementBuffer, 0);

	deviceContext->DSSetConstantBuffers(m_domainBufferResourceCount, 1, &m_displacementBuffer);

	m_domainBufferResourceCount++;

	result = deviceContext->Map(m_renderModeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* renderModeBufferDataPointer = static_cast<RenderModeBufferType*>(m_mappedResource.pData);

	renderModeBufferDataPointer->nonTexture = m_nonTextureRenderMode;
	renderModeBufferDataPointer->textureDiffuse = m_textureDiffuseRenderMode;
	renderModeBufferDataPointer->padding = XMFLOAT2();

	renderModeBufferDataPointer = nullptr;

	deviceContext->Unmap(m_renderModeBuffer, 0);

	deviceContext->PSSetConstantBuffers(m_pixelBufferResourceCount, 1, &m_renderModeBuffer);

	m_pixelBufferResourceCount++;

	return true;
}

void Shader::SetShader(ID3D11DeviceContext* const deviceContext) const {
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->HSSetShader(m_hullShader, nullptr, 0);
	deviceContext->DSSetShader(m_domainShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
}

void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND const hwnd, const LPCSTR& shaderFileName) const {

	ofstream out;

	//pointer to the error message
	auto* const compileErrors = static_cast<char*>(errorMessage->GetBufferPointer());

	//length of error message
	const auto bufferSize = errorMessage->GetBufferSize();

	//Write error to text file
	out.open("shader-error.txt");

	for (unsigned long i = 0; i < bufferSize; i++)
	{
		out << compileErrors[i];
	}

	out.close();

	//Release resources
	errorMessage->Release();
	errorMessage = nullptr;

	MessageBox(hwnd, "Error Compiling Shader. Check shader-error.txt for message.", shaderFileName, MB_OK);
}
