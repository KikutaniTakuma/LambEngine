#include "MeshManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include "../ResourceManager/ResourceManager.h"

MeshManager* MeshManager::instance_ = nullptr;

MeshManager* const MeshManager::GetInstance() {
	assert(instance_);
	return instance_;
}

MeshManager::~MeshManager() {
	meshs_.clear();
}

void MeshManager::Initialize() {
	instance_ = new MeshManager{};
	if (instance_) {
		Lamb::AddLog("Initialize MeshManager succeeded");
	}
	else {
		throw Lamb::Error::Code<MeshManager>("instance is nullptr", __func__);
	}
}
void MeshManager::Finalize() {
	Lamb::SafeDelete(instance_);
}

Mesh* MeshManager::LoadObj(const std::string& objFileName) {
	auto mesh = meshs_.find(objFileName);

	if (mesh == meshs_.end()) {
		meshs_[objFileName];
		meshs_[objFileName] = std::make_unique<Mesh>();
		meshs_[objFileName]->LoadObj(objFileName);
		
		ResourceManager::GetInstance()->SetModelResource(objFileName);
	}

	return meshs_[objFileName].get();
}

void MeshManager::Unload(const std::string& objFileName)
{
	auto isExist = meshs_.find(objFileName);
	if (isExist != meshs_.end()) {
		if (meshs_[objFileName]) {
			meshs_[objFileName].reset();
		}
		meshs_.erase(objFileName);
	}
}

void MeshManager::ResetDrawCount() {
	for (auto& i : meshs_) {
		i.second->ResetDrawCount();
	}
}

void MeshManager::Draw() {
	for (auto& i : meshs_) {
		i.second->Draw();
	}
}