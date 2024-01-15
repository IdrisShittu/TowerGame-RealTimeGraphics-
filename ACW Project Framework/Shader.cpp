#include "Shader.h"

Shader::Shader(const string& vertexShaderFileName, const string& hullShaderFileName, const string& domainShaderFileName, const string& pixelShaderFileName, ID3D11Device* const device, HWND const hwnd) : initializationFailed(false), vertexBufferResourceCount(0), hullBufferResourceCount(0), domainBufferResourceCount(0), pixelBufferResourceCount(0), nonTextureRenderMode(0), textureDiffuseRenderMode(0), displacementRenderMode(0), maxTessellationDistance(1.0f), minTessellationDistance(1.0f), maxTessellationFactor(0.0f), minTessellationFactor(0.0f), mipInterval(0.0f), mipClampMinimum(0.0f), mipClampMaximum(0.0f), displacementPower(0.0f), vertexShaderBuffer(nullptr), vertexShader(nullptr), hullShader(nullptr), domainShader(nullptr), pixelShader(nullptr), matrixBuffer(nullptr), tessellationBuffer(nullptr), cameraBuffer(nullptr), renderModeBuffer(nullptr)
{
	ID3D10Blob* errorMessage = nullptr;

	const unsigned int numberOfElements = 0;

	//Load our Vertex and Pixel Shader

	const auto hlslVertexFileName = vertexShaderFileName + ".hlsl";

	auto result = D3DCompileFromFile(CA2W(hlslVertexFileName.c_str()), nullptr, nullptr, vertexShaderFileName.c_str(), "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, /*D3DCOMPILE_DEBUG*/ 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		initializationFailed = true;

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
		initializationFailed = true;

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
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &vertexShader);

	if (FAILED(result))
	{
		initializationFailed = true;
		return;
	}

	result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), nullptr, &hullShader);

	if (FAILED(result))
	{
		initializationFailed = true;
		return;
	}

	result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), nullptr, &domainShader);

	if (FAILED(result))
	{
		initializationFailed = true;
		return;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &pixelShader);

	if (FAILED(result))
	{
		initializationFailed = true;
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
	result = device->CreateBuffer(&matrixBufferDescription, nullptr, &matrixBuffer);

	if (FAILED(result))
	{
		initializationFailed = true;
		return;
	}

	D3D11_BUFFER_DESC tessellationBufferDescription;

	tessellationBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDescription.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDescription.MiscFlags = 0;
	tessellationBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&tessellationBufferDescription, nullptr, &tessellationBuffer);

	if (FAILED(result))
	{
		initializationFailed = true;
		return;
	}

	D3D11_BUFFER_DESC cameraBufferDescription;

	cameraBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDescription.ByteWidth = sizeof(CameraBufferType); // Is a multiple of 16 because our extra float is inside
	cameraBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDescription.MiscFlags = 0;
	cameraBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&cameraBufferDescription, nullptr, &cameraBuffer);

	if (FAILED(result))
	{
		initializationFailed = true;
		return;
	}

	D3D11_BUFFER_DESC displacementBufferDescription;

	displacementBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	displacementBufferDescription.ByteWidth = sizeof(DisplacementBuffer);
	displacementBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	displacementBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	displacementBufferDescription.MiscFlags = 0;
	displacementBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&displacementBufferDescription, nullptr, &displacementBuffer);

	if (FAILED(result))
	{
		initializationFailed = true;
		return;
	}

	D3D11_BUFFER_DESC renderModeBufferDescription;

	renderModeBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	renderModeBufferDescription.ByteWidth = sizeof(RenderModeBufferType);
	renderModeBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	renderModeBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	renderModeBufferDescription.MiscFlags = 0;
	renderModeBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&renderModeBufferDescription, nullptr, &renderModeBuffer);

	if (FAILED(result))
	{
		initializationFailed = true;
		return;
	}
}

Shader::Shader(const Shader& other) = default;

Shader::Shader(Shader&& other) noexcept = default;

Shader::~Shader() {

	try
	{
		if (renderModeBuffer)
		{
			renderModeBuffer->Release();
			renderModeBuffer = nullptr;
		}

		if (cameraBuffer)
		{
			cameraBuffer->Release();
			cameraBuffer = nullptr;
		}

		if (tessellationBuffer)
		{
			tessellationBuffer->Release();
			tessellationBuffer = nullptr;
		}

		if (matrixBuffer)
		{
			matrixBuffer->Release();
			matrixBuffer = nullptr;
		}

		if (pixelShader)
		{
			pixelShader->Release();
			pixelShader = nullptr;
		}

		if (domainShader)
		{
			domainShader->Release();
			domainShader = nullptr;
		}

		if (hullShader)
		{
			hullShader->Release();
			hullShader = nullptr;
		}

		if (vertexShader)
		{
			vertexShader->Release();
			vertexShader = nullptr;
		}

		if (vertexShaderBuffer)
		{
			vertexShaderBuffer->Release();
			vertexShaderBuffer = nullptr;
		}
	}
	catch (exception& e)
	{
		
	}
}

Shader& Shader::operator=(const Shader& other) = default;

Shader& Shader::operator=(Shader&& other) noexcept = default;


bool Shader::GetInitializationState() const {
	return initializationFailed;
}

int Shader::GetVertexBufferResourceCount() const {
	return vertexBufferResourceCount;
}

int Shader::GetHullBufferResourceCount() const {
	return hullBufferResourceCount;
}

int Shader::GetDomainBufferResourceCount() const {
	return domainBufferResourceCount;
}

int Shader::GetPixelBufferResourceCount() const {
	return pixelBufferResourceCount;
}

void Shader::IncrementVertexBufferResourceCount() {
	vertexBufferResourceCount++;
}

void Shader::IncrementHullBufferResourceCount() {
	hullBufferResourceCount++;
}

void Shader::IncrementDomainBufferResourceCount() {
	domainBufferResourceCount++;
}

void Shader::IncrementPixelBufferResourceCount() {
	pixelBufferResourceCount++;
}

ID3D10Blob* Shader::GetVertexShaderBuffer() const
{
	return vertexShaderBuffer;
}

ID3D11Buffer* Shader::GetMatrixBuffer() const
{
	return matrixBuffer;
}

ID3D11Buffer* Shader::GetCameraBuffer() const {
	return cameraBuffer;
}

const D3D11_MAPPED_SUBRESOURCE& Shader::GetMappedSubResource() const
{
	return mappedResource;
}

void Shader::SetRenderModeStates(const int nonTextured, const int texturedDiffuse, const int displacementEnabled)
{
	nonTextureRenderMode = nonTextured;
	textureDiffuseRenderMode = texturedDiffuse;
	displacementRenderMode = displacementEnabled;
}

void Shader::GetTessellationVariables(float& maxTessellationDistance, float& minTessellationDistance, float& maxTessellationFactor, float& minTessellationFactor) const
{
	maxTessellationDistance = maxTessellationDistance;
	minTessellationDistance = minTessellationDistance;
	maxTessellationFactor = maxTessellationFactor;
	minTessellationFactor = minTessellationFactor;
}

void Shader::SetTessellationVariables(const float& maxTessellationDistance, const float& minTessellationDistance, const float& maxTessellationFactor, const float& minTessellationFactor)
{
	maxTessellationDistance = maxTessellationDistance;
	minTessellationDistance = minTessellationDistance;
	maxTessellationFactor = maxTessellationFactor;
	minTessellationFactor = minTessellationFactor;
}

void Shader::GetDisplacementVariables(float& mipInterval, float& mipClampMinimum, float& mipClampMaximum, float& displacementPower) const
{
	mipInterval = mipInterval;
	mipClampMinimum = mipClampMinimum;
	mipClampMaximum = mipClampMaximum;
	displacementPower = displacementPower;
}

void Shader::SetDisplacementVariables(const float& mipInterval, const float& mipClampMinimum, const float& mipClampMaximum, const float& displacementPower)
{
	mipInterval = mipInterval;
	mipClampMinimum = mipClampMinimum;
	mipClampMaximum = mipClampMaximum;
	displacementPower = displacementPower;
}

void Shader::SetInitializationState(const bool state)
{
	initializationFailed = state;
}

void Shader::SetVertexShaderBuffer(ID3D10Blob* const vertexShaderBuffer)
{
	vertexShaderBuffer = vertexShaderBuffer;
}


bool Shader::SetShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) {
	
	vertexBufferResourceCount = 0;
	hullBufferResourceCount = 0;
	domainBufferResourceCount = 0;
	pixelBufferResourceCount = 0;

	//Lock matrix constant buffer and set the transposed matrices to it
	auto result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* matrixVertexBufferDataPointer = static_cast<MatrixBufferType*>(mappedResource.pData);

	//This will be our world matrix inverse
	matrixVertexBufferDataPointer->viewMatrix = XMMatrixTranspose(XMMATRIX());
	//This will be our texture transform scale matrix
	matrixVertexBufferDataPointer->projectionMatrix = XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 0.0f));

	matrixVertexBufferDataPointer = nullptr;

	//Unlock constant buffer
	deviceContext->Unmap(matrixBuffer, 0);

	//Set the updated constant buffer
	deviceContext->VSSetConstantBuffers(vertexBufferResourceCount, 1, &matrixBuffer);

	vertexBufferResourceCount++;

	//Lock matrix constant buffer and set the transposed matrices to it
	result = deviceContext->Map(tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* tessellationBufferDataPointer = static_cast<TessellationBufferType*>(mappedResource.pData);


	tessellationBufferDataPointer->maxTessellationDistance = maxTessellationDistance;
	tessellationBufferDataPointer->minTessellationDistance = minTessellationDistance;
	tessellationBufferDataPointer->maxTessellationFactor = maxTessellationFactor;
	tessellationBufferDataPointer->minTessellationFactor = minTessellationFactor;

	tessellationBufferDataPointer = nullptr;

	//Unlock constant buffer
	deviceContext->Unmap(tessellationBuffer, 0);

	//Set the updated constant buffer
	deviceContext->VSSetConstantBuffers(vertexBufferResourceCount, 1, &tessellationBuffer);

	vertexBufferResourceCount++;

	//Lock camera constant buffer
	result = deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* cameraBufferDataPointer = static_cast<CameraBufferType*>(mappedResource.pData);

	cameraBufferDataPointer->cameraPosition = cameraPosition;
	cameraBufferDataPointer->padding = 0.0f;

	cameraBufferDataPointer = nullptr;

	//Unlock constant buffer
	deviceContext->Unmap(cameraBuffer, 0);

	//Set camera constant buffer in the vertex shader and DomainShader
	deviceContext->VSSetConstantBuffers(vertexBufferResourceCount, 1, &cameraBuffer);

	vertexBufferResourceCount++;

	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* matrixDomainBufferDataPointer = static_cast<MatrixBufferType*>(mappedResource.pData);

	matrixDomainBufferDataPointer->viewMatrix = XMMatrixTranspose(viewMatrix);
	matrixDomainBufferDataPointer->projectionMatrix = XMMatrixTranspose(projectionMatrix);

	matrixDomainBufferDataPointer = nullptr;

	deviceContext->Unmap(matrixBuffer, 0);

	deviceContext->DSSetConstantBuffers(domainBufferResourceCount, 1, &matrixBuffer);

	domainBufferResourceCount++;

	//Lock matrix constant buffer and set the transposed matrices to it
	result = deviceContext->Map(displacementBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* displacementBufferDataPointer = static_cast<DisplacementBuffer*>(mappedResource.pData);

	displacementBufferDataPointer->mipInterval = mipInterval;
	displacementBufferDataPointer->mipMinimum = mipClampMinimum;
	displacementBufferDataPointer->mipMaximum = mipClampMaximum;
	displacementBufferDataPointer->displacementPower = displacementPower;
	displacementBufferDataPointer->displacementEnabled = displacementRenderMode;
	displacementBufferDataPointer->padding = XMFLOAT3();

	displacementBufferDataPointer = nullptr;

	deviceContext->Unmap(displacementBuffer, 0);

	deviceContext->DSSetConstantBuffers(domainBufferResourceCount, 1, &displacementBuffer);

	domainBufferResourceCount++;

	result = deviceContext->Map(renderModeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* renderModeBufferDataPointer = static_cast<RenderModeBufferType*>(mappedResource.pData);

	renderModeBufferDataPointer->nonTexture = nonTextureRenderMode;
	renderModeBufferDataPointer->textureDiffuse = textureDiffuseRenderMode;
	renderModeBufferDataPointer->padding = XMFLOAT2();

	renderModeBufferDataPointer = nullptr;

	deviceContext->Unmap(renderModeBuffer, 0);

	deviceContext->PSSetConstantBuffers(pixelBufferResourceCount, 1, &renderModeBuffer);

	pixelBufferResourceCount++;

	return true;
}

void Shader::SetShader(ID3D11DeviceContext* const deviceContext) const {
	deviceContext->VSSetShader(vertexShader, nullptr, 0);
	deviceContext->HSSetShader(hullShader, nullptr, 0);
	deviceContext->DSSetShader(domainShader, nullptr, 0);
	deviceContext->PSSetShader(pixelShader, nullptr, 0);
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
