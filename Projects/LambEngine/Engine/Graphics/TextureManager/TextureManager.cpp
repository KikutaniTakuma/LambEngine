#include "TextureManager.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Utils/SafeDelete/SafeDelete.h"
#include <cassert>
#include <filesystem>

#include "Error/Error.h"

TextureManager* TextureManager::instance_ = nullptr;

TextureManager* const TextureManager::GetInstance() {
	return instance_;
}

void TextureManager::Initialize() {
	instance_ = new TextureManager();
	assert(instance_);
	instance_->LoadTexture("./Resources/white2x2.png");
}

void TextureManager::Finalize() {
	Lamb::SafeDelete(instance_);
}

TextureManager::TextureManager() :
	textures_(),
	thisFrameLoadFlg_(false)
{
	srvHeap_ = CbvSrvUavHeap::GetInstance();

	Lamb::AddLog("Initialize TextureManager succeeded");
}

TextureManager::~TextureManager() {
	textures_.clear();
}


Texture* const TextureManager::LoadTexture(const std::string& fileName) {
	if (!std::filesystem::exists(fileName)) {
		throw Lamb::Error::Code<TextureManager>("this file is not exist -> " + fileName, __func__);
	}

	auto itr = textures_.find(fileName);
	if (itr == textures_.end()) {
		DirectXCommand* const directXCommand = DirectXCommand::GetInstance();
		auto tex = std::make_unique<Texture>();
		tex->Load(fileName, directXCommand->GetCommandList());

		if (!tex->isLoad_) {
			return nullptr;
		}

		srvHeap_->CreateView(*tex);

		textures_.insert(std::make_pair(fileName, std::move(tex)));

		thisFrameLoadFlg_ = true;
	}

	return textures_[fileName].get();
}

Texture* const TextureManager::GetWhiteTex() {
	return instance_->textures_["./Resources/white2x2.png"].get();
}

void TextureManager::ReleaseIntermediateResource() {
	if (thisFrameLoadFlg_) {
		for (auto& i : textures_) {
			i.second->ReleaseIntermediateResource();
		}

		thisFrameLoadFlg_ = false;
	}
}

void TextureManager::Use(uint32_t texIndex, UINT rootParam) {
	auto* const mainComlist = DirectXCommand::GetInstance()->GetCommandList();
	mainComlist->SetGraphicsRootDescriptorTable(
		rootParam, srvHeap_->GetGpuHeapHandle(texIndex));
}