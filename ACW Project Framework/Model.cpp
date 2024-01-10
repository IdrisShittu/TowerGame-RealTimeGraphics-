#include "Model.h"

Model::Model(ID3D11Device* const device, const char* const modelFileName, const shared_ptr<ResourceManager>& resourceManager) : m_initializationFailed(false), m_bufferDescriptionSizeChange(false), m_updateInstanceBuffer(false), m_sizeOfVertexType(0), m_indexCount(0), m_instanceCount(0), m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_instanceBuffer(nullptr), m_instances(nullptr), m_instanceBufferDescription(nullptr), m_instanceData(nullptr)
{
	const auto result = resourceManager->GetModel(device, modelFileName, m_vertexBuffer, m_indexBuffer);

	if (!result)
	{
		m_initializationFailed = true;
		return;
	}

	m_sizeOfVertexType = resourceManager->GetSizeOfVertexType();
	m_indexCount = resourceManager->GetIndexCount(modelFileName);
}

Model::Model(ID3D11Device* const device, const char* const modelFileName, const shared_ptr<ResourceManager>& resourceManager, const vector<XMFLOAT3> &scales, const vector<XMFLOAT3> &rotations, const vector<XMFLOAT3> &positions) : Model(device, modelFileName, resourceManager)
{
	//Set the number of instances we have
	m_instanceCount = positions.size();

	m_instances  = new InstanceType[m_instanceCount];

	if (!m_instances)
	{
		delete[] m_instances;
		m_instances = nullptr;

		m_initializationFailed = true;
		return;
	}

	//Construct world matrixes
	for (unsigned int i = 0; i < positions.size(); i++)
	{
		auto worldMatrix = XMMatrixIdentity();

		const auto position = positions[i];
		const auto scale = i < scales.size() ? scales[i] : scales[scales.size() - 1];
		const auto rotation = i < rotations.size() ? rotations[i] : rotations[rotations.size() - 1];

		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(scale.x, scale.y, scale.z));
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)));
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(position.x, position.y, position.z));

		m_instances[i].worldMatrix = XMMatrixTranspose(worldMatrix);
	}

	m_updateInstanceBuffer = true;

	//Set up instance buffer description
	m_instanceBufferDescription = make_shared<D3D11_BUFFER_DESC>();

	m_instanceBufferDescription->Usage = D3D11_USAGE_DYNAMIC;
	m_instanceBufferDescription->ByteWidth = sizeof(InstanceType) * m_instanceCount;
	m_instanceBufferDescription->BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_instanceBufferDescription->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_instanceBufferDescription->MiscFlags = 0;
	m_instanceBufferDescription->StructureByteStride = 0;

	//Create instance buffer
	const auto result = device->CreateBuffer(m_instanceBufferDescription.get(), nullptr, &m_instanceBuffer);

	if (FAILED(result))
	{
		delete[] m_instances;
		m_instances = nullptr;

		m_initializationFailed = true;
		return;
	}
}

Model::Model(const Model& other) = default;

Model::Model(Model&& other) noexcept = default;

Model::~Model()
{
	try
	{
		//Release resources
		if (m_instances)
		{
			delete[] m_instances;
			m_instances = nullptr;
		}

		if (m_instanceBuffer)
		{
			m_instanceBuffer->Release();
			m_instanceBuffer = nullptr;
		}

		if (m_indexBuffer)
		{
			//Don't release, resource manager does this
			//m_indexBuffer->Release();
			m_indexBuffer = nullptr;
		}

		if (m_vertexBuffer)
		{
			//Don't release, resource manager does this
			//m_vertexBuffer->Release();
			m_vertexBuffer = nullptr;
		}
	}
	catch (exception& e)
	{
		
	}
}

Model& Model::operator=(const Model& other) = default;

Model& Model::operator=(Model&& other) noexcept = default;

void Model::Update(const vector<XMFLOAT3> &scales, const vector<XMFLOAT3> &rotations, const vector<XMFLOAT3> &positions, const XMMATRIX& parentMatrix)
{
	if (m_instanceCount != positions.size())
	{
		m_instanceCount = positions.size();

		delete[] m_instances;
		m_instances = nullptr;

		m_instances = new InstanceType[m_instanceCount];

		m_bufferDescriptionSizeChange = true;
	}

	//Construct world matrixes
	for (unsigned int i = 0; i < m_instanceCount; i++)
	{
		auto worldMatrix = XMMatrixIdentity();

		const auto position = positions[i];
		const auto scale = i < scales.size() ? scales[i] : scales[scales.size() - 1];
		const auto rotation = i < rotations.size() ? rotations[i] : rotations[rotations.size() - 1];

		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(scale.x, scale.y, scale.z));
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)));
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(position.x, position.y, position.z));

		worldMatrix = worldMatrix * parentMatrix;

		m_instances[i].worldMatrix = XMMatrixTranspose(worldMatrix);
	}

	m_updateInstanceBuffer = true;
}


bool Model::Render(ID3D11DeviceContext* const deviceContext) {

	if (m_updateInstanceBuffer)
	{
		if (m_bufferDescriptionSizeChange)
		{
			ID3D11Device* device;

			deviceContext->GetDevice(&device);

			m_instanceBuffer->Release();

			m_instanceBufferDescription->ByteWidth = sizeof(InstanceType) * m_instanceCount;

			const auto result = device->CreateBuffer(m_instanceBufferDescription.get(), nullptr, &m_instanceBuffer);

			if (FAILED(result))
			{
				return false;
			}

			m_bufferDescriptionSizeChange = false;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		const auto hresult = deviceContext->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (FAILED(hresult))
		{
			return false;
		}

		//auto* matrixBufferData = static_cast<InstanceType*>(mappedResource.pData);

		memcpy(mappedResource.pData, m_instances, m_instanceCount * sizeof(InstanceType));

		deviceContext->Unmap(m_instanceBuffer, 0);

		m_updateInstanceBuffer = false;
	}

	//Render buffers

	//Set vertex buffer stride and offset
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	strides[0] = m_sizeOfVertexType;
	strides[1] = sizeof(InstanceType);

	offsets[0] = 0;
	offsets[1] = 0;

	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_instanceBuffer;

	//Set the vertex buffer to active in the input assembler so it will render it
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	//Set the index buffer to active in the input assembler so it will render it
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive render style for the vertex buffer
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	return true;
}

int Model::GetIndexCount() const {
	return m_indexCount;
}

int Model::GetInstanceCount() const
{
	return m_instanceCount;
}

bool Model::GetInitializationState() const {
	return m_initializationFailed;
}

