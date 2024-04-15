#include "DrawerManager.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

DrawerManager* DrawerManager::instance_ = nullptr;

DrawerManager::DrawerManager():
	tex2D_()
{
	tex2D_ = std::make_unique<Texture2D>();
	tex2D_->Load();
}

DrawerManager* DrawerManager::GetInstance()
{
	return instance_;
}

void DrawerManager::Initialize() {
	instance_ = new DrawerManager();
}

void DrawerManager::Finalize() {
	Lamb::SafeDelete(instance_);
}

Texture2D* const DrawerManager::GetTexture2D() const
{
	return tex2D_.get();
}

void DrawerManager::LoadModel(const std::string& fileName) {
	auto isExist = models_.find(fileName);

	if (isExist == models_.end()) {
		std::unique_ptr<Model> newModel = std::make_unique<Model>(fileName);

		models_.insert(std::make_pair(fileName, newModel.release()));
	}
}

Model* const DrawerManager::GetModel(const std::string& fileName)
{
	if (not models_[fileName]) {
		throw Lamb::Error::Code<DrawerManager>("this model is not loaded -> " + fileName, __func__);
	}

	return models_[fileName].get();
}


