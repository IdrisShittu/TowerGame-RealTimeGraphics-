#include "Model.h"

Model::Model(ID3D11Device* const device, const char* const modelFileName, const shared_ptr<ResourceManager>& resourceManager) : initializationFailed(false), bufferDescriptionSizeChange(false), updateInstanceBuffer(false), sizeOfVertexType(0), indexCount(0), instanceCount(0), vertexBuffer(nullptr), indexBuffer(nullptr), instanceBuffer(nullptr), instances(nullptr), instanceBufferDescription(nullptr), instanceData(nullptr)
{
	const auto result = resourceManager->GetModel(device, modelFileName, vertexBuffer, indexBuffer);

	if (!result)
	{
		initializationFailed = true;
		return;
	}

	sizeOfVertexType = resourceManager->GetSizeOfVertexType();
	indexCount = resourceManager->GetIndexCount(modelFileName);
}

Model::Model(ID3D11Device* const device, const char* const modelFileName, const shared_ptr<ResourceManager>& resourceManager, const vector<XMFLOAT3> &scales, const vector<XMFLOAT3> &rotations, const vector<XMFLOAT3> &positions) : Model(device, modelFileName, resourceManager)
{
	//Set the number of instances we have
	instanceCount = positions.size();

	instances  = new InstanceType[instanceCount];

	if (!instances)
	{
		delete[] instances;
		instances = nullptr;

		initializationFailed = true;
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

		instances[i].worldMatrix = XMMatrixTranspose(worldMatrix);
	}

	updateInstanceBuffer = true;

	//Set up instance buffer description
	instanceBufferDescription = make_shared<D3D11_BUFFER_DESC>();

	instanceBufferDescription->Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDescription->ByteWidth = sizeof(InstanceType) * instanceCount;
	instanceBufferDescription->BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDescription->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDescription->MiscFlags = 0;
	instanceBufferDescription->StructureByteStride = 0;

	//Create instance buffer
	const auto result = device->CreateBuffer(instanceBufferDescription.get(), nullptr, &instanceBuffer);

	if (FAILED(result))
	{
		delete[] instances;
		instances = nullptr;

		initializationFailed = true;
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
		if (instances)
		{
			delete[] instances;
			instances = nullptr;
		}

		if (instanceBuffer)
		{
			instanceBuffer->Release();
			instanceBuffer = nullptr;
		}

		if (indexBuffer)
		{
			//Don't release, resource manager does this
			//indexBuffer->Release();
			indexBuffer = nullptr;
		}

		if (vertexBuffer)
		{
			//Don't release, resource manager does this
			//vertexBuffer->Release();
			vertexBuffer = nullptr;
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
	if (instanceCount != positions.size())
	{
		instanceCount = positions.size();

		delete[] instances;
		instances = nullptr;

		instances = new InstanceType[instanceCount];

		bufferDescriptionSizeChange = true;
	}

	//Construct world matrixes
	for (unsigned int i = 0; i < instanceCount; i++)
	{
		auto worldMatrix = XMMatrixIdentity();

		const auto position = positions[i];
		const auto scale = i < scales.size() ? scales[i] : scales[scales.size() - 1];
		const auto rotation = i < rotations.size() ? rotations[i] : rotations[rotations.size() - 1];

		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(scale.x, scale.y, scale.z));
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)));
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(position.x, position.y, position.z));

		worldMatrix = worldMatrix * parentMatrix;

		instances[i].worldMatrix = XMMatrixTranspose(worldMatrix);
	}

	updateInstanceBuffer = true;
}


bool Model::Render(ID3D11DeviceContext* const deviceContext) {

	if (updateInstanceBuffer)
	{
		if (bufferDescriptionSizeChange)
		{
			ID3D11Device* device;

			deviceContext->GetDevice(&device);

			instanceBuffer->Release();

			instanceBufferDescription->ByteWidth = sizeof(InstanceType) * instanceCount;

			const auto result = device->CreateBuffer(instanceBufferDescription.get(), nullptr, &instanceBuffer);

			if (FAILED(result))
			{
				return false;
			}

			bufferDescriptionSizeChange = false;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		const auto hresult = deviceContext->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (FAILED(hresult))
		{
			return false;
		}

		//auto* matrixBufferData = static_cast<InstanceType*>(mappedResource.pData);

		memcpy(mappedResource.pData, instances, instanceCount * sizeof(InstanceType));

		deviceContext->Unmap(instanceBuffer, 0);

		updateInstanceBuffer = false;
	}

	//Render buffers

	//Set vertex buffer stride and offset
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	strides[0] = sizeOfVertexType;
	strides[1] = sizeof(InstanceType);

	offsets[0] = 0;
	offsets[1] = 0;

	bufferPointers[0] = vertexBuffer;
	bufferPointers[1] = instanceBuffer;

	//Set the vertex buffer to active in the input assembler so it will render it
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	//Set the index buffer to active in the input assembler so it will render it
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive render style for the vertex buffer
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	return true;
}

int Model::GetIndexCount() const {
	return indexCount;
}

int Model::GetInstanceCount() const
{
	return instanceCount;
}

bool Model::GetInitializationState() const {
	return initializationFailed;
}

