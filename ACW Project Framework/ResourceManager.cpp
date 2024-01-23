#include "ResourceManager.h"

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

bool ResourceManager::LoadModel(ID3D11Device* const device, const char* const modelFileName) {
    ifstream fin(modelFileName);
    if (fin.fail()) {
        return false;
    }

    vector<VertexType> vertices;
    vector<unsigned long> indices;
    vector<XMFLOAT3> positions, normals;
    vector<XMFLOAT2> textures;

    char cmd[256];
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
            unsigned long index[3];
            for (int i = 0; i < 3; ++i) {
                fin >> index[i];
                vertices.push_back({ positions[index[i] - 1], normals[index[i] - 1], textures[index[i] - 1] });
                indices.push_back(index[i] - 1);
            }
        }
    }

    // Create and set vertex and index buffers
    if (!CreateBuffers(device, vertices, indices, modelFileName)) {
        return false;
    }

    return true;
}

bool ResourceManager::CreateBuffers(ID3D11Device* const device, const vector<VertexType>& vertices, const vector<unsigned long>& indices, const char* const modelFileName) {
    D3D11_BUFFER_DESC vertexBufferDesc = { sizeof(VertexType) * vertices.size(), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
    D3D11_SUBRESOURCE_DATA vertexData = { vertices.data(), 0, 0 };

    ID3D11Buffer* vertexBuffer;
    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer))) {
        return false;
    }

    D3D11_BUFFER_DESC indexBufferDesc = { sizeof(unsigned long) * indices.size(), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
    D3D11_SUBRESOURCE_DATA indexData = { indices.data(), 0, 0 };

    ID3D11Buffer* indexBuffer;
    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer))) {
        vertexBuffer->Release();
        return false;
    }

    vertexBuffers[modelFileName] = vertexBuffer;
    indexBuffers[modelFileName] = indexBuffer;
    indexCount[modelFileName] = indices.size();

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
