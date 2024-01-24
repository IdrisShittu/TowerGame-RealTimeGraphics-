#include "ResourceManager.h"
#include <iostream>

#include <string>

#include <sstream>
ResourceManager::ResourceManager()
{

}

ResourceManager::ResourceManager(const ResourceManager& other) = default;

//ResourceManager::ResourceManager(ResourceManager&& other) noexcept = default;

ResourceManager::~ResourceManager() {
	auto releaseResources = [](auto& resources) {
		for (auto& resource : resources) {
			if (resource.second) {
				resource.second->Release();
				resource.second = nullptr;
			}
		}
	};

	releaseResources(textures);
	releaseResources(indexBuffers);
	releaseResources(vertexBuffers);
}


ResourceManager& ResourceManager::operator=(const ResourceManager& other) = default;

ResourceManager& ResourceManager::operator=(ResourceManager&& other) noexcept = default;

bool ResourceManager::GetModel(ID3D11Device* const device, const char* const modelFileName, ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer) {
	if (vertexBuffers.count(modelFileName) == 0 && !LoadModel(device, modelFileName)) return false;
	vertexBuffer = vertexBuffers[modelFileName];
	indexBuffer = indexBuffers[modelFileName];
	return true;
}


bool ResourceManager::GetTexture(ID3D11Device* const device, const WCHAR* const textureFileName, ID3D11ShaderResourceView*& texture) {
	if (textures.count(textureFileName) == 0 && !LoadTexture(device, textureFileName)) {
		return false;
	}

	texture = textures[textureFileName];
	return true;
}


int ResourceManager::GetSizeOfVertexType() const {
	return sizeof(VertexType);
}

int ResourceManager::GetIndexCount(const char* const modelFileName) const {
	return indexCount.at(modelFileName);
}

//bool ResourceManager::LoadModel(ID3D11Device* const device, const char* const modelFileName)
//{
//	//Load Model
//	ifstream fin;
//
//	//Open obj file
//	fin.open(modelFileName);
//
//	if (fin.fail())
//	{
//		return false;
//	}
//
//	ID3D11Buffer* vertexBuffer = nullptr;
//	ID3D11Buffer* indexBuffer = nullptr;
//
//	vector<XMFLOAT3> positions;
//	vector<XMFLOAT2> textures;
//	vector<XMFLOAT3> normals;
//
//	char cmd[256] = { 0 };
//
//	auto faceCount = 0;
//	auto vertexCount = 0;
//	auto indCount = 0;
//
//	VertexType* vertices = nullptr;
//	unsigned long* indices = nullptr;
//
//	while (!fin.eof())
//	{
//		float x, y, z;
//
//		fin >> cmd;
//
//		if (0 == strcmp(cmd, "faces"))
//		{
//			fin >> faceCount;
//			vertices = new VertexType[faceCount * 3];
//
//			if (!vertices)
//			{
//				return false;
//			}
//
//			vertexCount = faceCount * 3;
//
//			indices = new unsigned long[faceCount * 3];
//
//			if (!indices)
//			{
//				return false;
//			}
//
//			indCount = faceCount * 3;
//		}
//
//		if (0 == strcmp(cmd, "v"))
//		{
//			fin >> x >> y >> z;
//			positions.emplace_back(XMFLOAT3(x, y, z));
//		}
//		else if (0 == strcmp(cmd, "vn"))
//		{
//			fin >> x >> y >> z;
//			normals.emplace_back(XMFLOAT3(x, y, z));
//		}
//		else if (0 == strcmp(cmd, "vt"))
//		{
//			fin >> x >> y >> z;
//			textures.emplace_back(XMFLOAT2(x, y));
//		}
//		else if (0 == strcmp(cmd, "f"))
//		{
//			int value;
//			auto count = 0;
//
//			while (0 == strcmp(cmd, "f"))
//			{
//				VertexType* tempVertexFace[3];
//
//				for (auto i = 0; i < 3; i++)
//				{
//					fin >> value;
//					vertices[count].position = (positions[value - 1]);
//					fin.ignore();
//
//					fin >> value;
//					vertices[count].texture = (textures[value - 1]);
//					fin.ignore();
//
//					fin >> value;
//					vertices[count].normal = (normals[value - 1]);
//					fin.ignore();
//
//					indices[count] = count;
//
//					tempVertexFace[i] = &vertices[count];
//
//					count++;
//				}
//
//				//Calculate the tangent and binormal
//
//				auto positionOne = XMFLOAT3();
//				auto positionTwo = XMFLOAT3();
//				auto textureOne = XMFLOAT2();
//				auto textureTwo = XMFLOAT2();
//
//				auto tangent = XMFLOAT3();
//				auto binormal = XMFLOAT3();
//
//				//Calculate the two vertex positions from the face
//				positionOne.x = tempVertexFace[1]->position.x - tempVertexFace[0]->position.x;
//				positionOne.y = tempVertexFace[1]->position.y - tempVertexFace[0]->position.y;
//				positionOne.z = tempVertexFace[1]->position.z - tempVertexFace[0]->position.z;
//
//				positionTwo.x = tempVertexFace[2]->position.x - tempVertexFace[0]->position.x;
//				positionTwo.y = tempVertexFace[2]->position.y - tempVertexFace[0]->position.y;
//				positionTwo.z = tempVertexFace[2]->position.z - tempVertexFace[0]->position.z;
//
//				//Calculate the two texture coords from the face
//				textureOne.x = tempVertexFace[1]->texture.x - tempVertexFace[0]->texture.x;
//				textureOne.y = tempVertexFace[1]->texture.y - tempVertexFace[0]->texture.y;
//
//				textureTwo.x = tempVertexFace[2]->texture.x - tempVertexFace[0]->texture.x;
//				textureTwo.y = tempVertexFace[2]->texture.y - tempVertexFace[0]->texture.y;
//
//				//Calculate the denominator of the tangent/binormal (This is so we don't have to normalize after, we can do it as we go along
//				const auto denominator = 1.0f / (textureOne.x * textureTwo.y - textureTwo.x * textureOne.y);
//
//				//Calculate the cross products and scale it by our denominator to get the normalize tangent and binormal
//				tangent.x = (textureTwo.y * positionOne.x - textureOne.y * positionTwo.x) * denominator;
//				tangent.y = (textureTwo.y * positionOne.y - textureOne.y * positionTwo.y) * denominator;
//				tangent.z = (textureTwo.y * positionOne.z - textureOne.y * positionTwo.z) * denominator;
//
//				binormal.x = (textureOne.x * positionTwo.x - textureTwo.x * positionOne.x) * denominator;
//				binormal.y = (textureOne.x * positionTwo.y - textureTwo.x * positionOne.y) * denominator;
//				binormal.z = (textureOne.x * positionTwo.z - textureTwo.x * positionOne.z) * denominator;
//
//				//Calculate the length of the tangent normal
//				auto length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
//
//				//Normalize our tangent based off the length
//				tangent.x = tangent.x / length;
//				tangent.y = tangent.y / length;
//				tangent.z = tangent.z / length;
//
//				//Calculate the length of the binormal
//				length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));
//
//				//Normalize it
//				binormal.x = binormal.x / length;
//				binormal.y = binormal.y / length;
//				binormal.z = binormal.z / length;
//
//				//Calculate new normal based off the tangent and binormal
//				auto newNormal = XMFLOAT3();
//
//				//Do a cross product between the tangent and binormal to get the new normal
//				newNormal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
//				newNormal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
//				newNormal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);
//
//				//Calculate length of normal
//				length = sqrt((newNormal.x * newNormal.x) + (newNormal.y * newNormal.y) + (newNormal.z * newNormal.z));
//
//				//Normalize it
//				newNormal.x = newNormal.x / length;
//				newNormal.y = newNormal.y / length;
//				newNormal.z = newNormal.z / length;
//
//				//Store new normal, tangent and binormal back into the face
//				tempVertexFace[0]->normal = newNormal;
//				tempVertexFace[0]->tangent = tangent;
//				tempVertexFace[0]->binormal = binormal;
//				tempVertexFace[0] = nullptr;
//
//				tempVertexFace[1]->normal = newNormal;
//				tempVertexFace[1]->tangent = tangent;
//				tempVertexFace[1]->binormal = binormal;
//				tempVertexFace[1] = nullptr;
//
//				tempVertexFace[2]->normal = newNormal;
//				tempVertexFace[2]->tangent = tangent;
//				tempVertexFace[2]->binormal = binormal;
//				tempVertexFace[2] = nullptr;
//
//				fin >> cmd;
//			}
//		}
//	}
//
//	if (!CreateBuffers(device, vertices, indices, vertexCount, indCount, &vertexBuffer, &indexBuffer)) {
//		delete[] vertices;
//		delete[] indices;
//		return false;
//	}
//
//	// Store the buffers in the resource manager
//	indexCount.insert(pair<const char*, int>(modelFileName, indCount));
//	vertexBuffers.insert(pair<const char*, ID3D11Buffer*>(modelFileName, vertexBuffer));
//	indexBuffers.insert(pair<const char*, ID3D11Buffer*>(modelFileName, indexBuffer));
//
//	// Clean up temporary buffers
//	delete[] vertices;
//	delete[] indices;
//
//	return true;
//	
//}
//
//bool ResourceManager::CreateBuffers(ID3D11Device* const device, VertexType* vertices, unsigned long* indices, int vertexCount, int indCount, ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer) {
//	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//	D3D11_SUBRESOURCE_DATA vertexData, indexData;
//
//	// Set up the description of the vertex buffer
//	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.CPUAccessFlags = 0;
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the vertex data
//	vertexData.pSysMem = vertices;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	// Now create the vertex buffer
//	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, vertexBuffer))) {
//		return false;
//	}
//
//	// Set up the description of the index buffer
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indCount;
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the index data
//	indexData.pSysMem = indices;
//	indexData.SysMemPitch = 0;
//	indexData.SysMemSlicePitch = 0;
//
//	// Create the index buffer
//	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, indexBuffer))) {
//		return false;
//	}
//
//	return true;
//}

bool ResourceManager::LoadModel(ID3D11Device* const device, const char* const modelFileName) {
    ifstream fin(modelFileName);
    if (!fin) return false;

    vector<XMFLOAT3> positions, normals;
    vector<XMFLOAT2> textures;
    vector<VertexType> vertices;
    vector<unsigned long> indices;

    char cmd[256] = { 0 };
    while (fin >> cmd) {
        if (strcmp(cmd, "v") == 0) {
            XMFLOAT3 position;
            fin >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (strcmp(cmd, "vn") == 0) {
            XMFLOAT3 normal;
            fin >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (strcmp(cmd, "vt") == 0) {
            XMFLOAT2 texture;
            fin >> texture.x >> texture.y;
            textures.push_back(texture);
        }
        else if (strcmp(cmd, "f") == 0) {
            ProcessFace(fin, positions, textures, normals, vertices, indices);
        }
    }

    ID3D11Buffer* vertexBuffer = nullptr, * indexBuffer = nullptr;
    if (!CreateBuffers(device, vertices, indices, &vertexBuffer, &indexBuffer)) {
        return false;
    }

    // Store the buffers
    indexCount[modelFileName] = indices.size();
    vertexBuffers[modelFileName] = vertexBuffer;
    indexBuffers[modelFileName] = indexBuffer;

    return true;
}

void ResourceManager::ProcessFace(ifstream& fin, const vector<XMFLOAT3>& positions, const vector<XMFLOAT2>& textures,
    const vector<XMFLOAT3>& normals, vector<VertexType>& vertices, vector<unsigned long>& indices) {
    char cmd[256];
    int value;
    VertexType* tempVertexFace[3];

    for (auto i = 0; i < 3; i++) {
        fin >> value;
        VertexType vertex;
        vertex.position = positions[value - 1];
        fin.ignore();

        fin >> value;
        vertex.texture = textures[value - 1];
        fin.ignore();

        fin >> value;
        vertex.normal = normals[value - 1];
        fin.ignore();

        tempVertexFace[i] = &vertex;
        vertices.push_back(vertex);
        indices.push_back(vertices.size() - 1);
    }

    // Calculate tangent and binormal
    CalculateTangentBinormal(tempVertexFace[0], tempVertexFace[1], tempVertexFace[2]);

    fin >> cmd; // Move to the next line (or next face)
}

void ResourceManager::CalculateTangentBinormal(VertexType* v0, VertexType* v1, VertexType* v2) {
    // Calculations for tangent and binormal as per your original logic
    XMFLOAT3 positionOne = XMFLOAT3(v1->position.x - v0->position.x, v1->position.y - v0->position.y, v1->position.z - v0->position.z);
    XMFLOAT3 positionTwo = XMFLOAT3(v2->position.x - v0->position.x, v2->position.y - v0->position.y, v2->position.z - v0->position.z);

    XMFLOAT2 textureOne = XMFLOAT2(v1->texture.x - v0->texture.x, v1->texture.y - v0->texture.y);
    XMFLOAT2 textureTwo = XMFLOAT2(v2->texture.x - v0->texture.x, v2->texture.y - v0->texture.y);

    float denominator = 1.0f / (textureOne.x * textureTwo.y - textureTwo.x * textureOne.y);

    XMFLOAT3 tangent = {
        (textureTwo.y * positionOne.x - textureOne.y * positionTwo.x) * denominator,
        (textureTwo.y * positionOne.y - textureOne.y * positionTwo.y) * denominator,
        (textureTwo.y * positionOne.z - textureOne.y * positionTwo.z) * denominator
    };

    XMFLOAT3 binormal = {
        (textureOne.x * positionTwo.x - textureTwo.x * positionOne.x) * denominator,
        (textureOne.x * positionTwo.y - textureTwo.x * positionOne.y) * denominator,
        (textureOne.x * positionTwo.z - textureTwo.x * positionOne.z) * denominator
    };

    NormalizeVector(tangent);
    NormalizeVector(binormal);

    // Update the tangent and binormal for each vertex in the face
    v0->tangent = tangent;
    v0->binormal = binormal;
    v1->tangent = tangent;
    v1->binormal = binormal;
    v2->tangent = tangent;
    v2->binormal = binormal;
}

void ResourceManager::NormalizeVector(XMFLOAT3& vector) {
    float length = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
    vector.x /= length;
    vector.y /= length;
    vector.z /= length;
}

bool ResourceManager::CreateBuffers(ID3D11Device* const device, const vector<VertexType>& vertices, const vector<unsigned long>& indices, ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer) {
    D3D11_BUFFER_DESC bufferDesc;
    D3D11_SUBRESOURCE_DATA subData;

    // Calculate the size of the vertex buffer
    size_t vertexBufferSize = sizeof(VertexType) * vertices.size();
    if (vertexBufferSize > UINT_MAX) {
        // Handle the error: the buffer size is too large
        return false;
    }

    // Create vertex buffer
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferSize);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    subData.pSysMem = vertices.data();
    subData.SysMemPitch = 0;
    subData.SysMemSlicePitch = 0;

    if (FAILED(device->CreateBuffer(&bufferDesc, &subData, vertexBuffer))) {
        return false;
    }

    // Calculate the size of the index buffer
    size_t indexBufferSize = sizeof(unsigned long) * indices.size();
    if (indexBufferSize > UINT_MAX) {
        // Handle the error: the buffer size is too large
        return false;
    }

    // Create index buffer
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = static_cast<UINT>(indexBufferSize);
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    subData.pSysMem = indices.data();
    subData.SysMemPitch = 0;
    subData.SysMemSlicePitch = 0;

    if (FAILED(device->CreateBuffer(&bufferDesc, &subData, indexBuffer))) {
        return false;
    }

    return true;
}

bool ResourceManager::LoadTexture(ID3D11Device* const device, const WCHAR* textureFileName) {
	ID3D11ShaderResourceView* texture = nullptr;
	HRESULT result = CreateDDSTextureFromFile(device, textureFileName, nullptr, &texture);

	if (SUCCEEDED(result)) {
		textures[textureFileName] = texture;
		return true;
	}
	delete texture;
	return false;
}
