#include "MeshManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include "../MeshLoader/MeshLoader.h"

Lamb::SafePtr<MeshManager> MeshManager::instance_ = nullptr;

MeshManager* const MeshManager::GetInstance() {
	return instance_.get();
}

MeshManager::~MeshManager() {
	meshs_.clear();
}

void MeshManager::Initialize() {
	instance_.reset(new MeshManager());
	if (instance_) {
		Lamb::AddLog("Initialize MeshManager succeeded");
	}
	else {
		throw Lamb::Error::Code<MeshManager>("instance is nullptr", ErrorPlace);
	}
}
void MeshManager::Finalize() {
	instance_.reset();
}

Mesh* MeshManager::LoadModel(const std::string& fileName) {
	auto mesh = meshs_.find(fileName);

	if (mesh == meshs_.end()) {
		modelData_.insert(std::make_pair(fileName, std::make_unique<ModelData>(MeshLoader::LoadModel(fileName))));
		meshs_.insert(std::make_pair(fileName, CreateMesh(*modelData_[fileName])));
	}

	return meshs_[fileName].get();
}

Mesh* MeshManager::CreateMesh(const ModelData& modelData)
{
	Lamb::SafePtr result = Lamb::MakeSafePtr<Mesh>();
	Lamb::SafePtr directXDevice = DirectXDevice::GetInstance();


	uint32_t indexSizeInBytes = static_cast<uint32_t>(sizeof(uint32_t) * modelData.indices.size());
	uint32_t vertexSizeInBytes = static_cast<uint32_t>(sizeof(Vertex) * modelData.vertices.size());

	result->node = modelData.rootNode;
	result->indexResource = directXDevice->CreateBufferResuorce(indexSizeInBytes);

	Lamb::SafePtr<uint32_t> indexMap = nullptr;
	result->indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	std::copy(modelData.indices.begin(), modelData.indices.end(), indexMap);
	result->indexResource->Unmap(0, nullptr);

	result->indexNumber = static_cast<uint32_t>(modelData.indices.size());
	result->indexView.SizeInBytes = indexSizeInBytes;
	result->indexView.Format = DXGI_FORMAT_R32_UINT;
	result->indexView.BufferLocation = result->indexResource->GetGPUVirtualAddress();


	result->vertexResource = directXDevice->CreateBufferResuorce(vertexSizeInBytes);

	Lamb::SafePtr<Vertex> vertMap = nullptr;
	result->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	std::copy(modelData.vertices.begin(), modelData.vertices.end(), vertMap);
	result->vertexResource->Unmap(0, nullptr);

	result->vertexNumber = static_cast<uint32_t>(modelData.vertices.size());
	result->vertexView.SizeInBytes = vertexSizeInBytes;
	result->vertexView.StrideInBytes = static_cast<uint32_t>(sizeof(Vertex));
	result->vertexView.BufferLocation = result->vertexResource->GetGPUVirtualAddress();

	return result.release();
}
