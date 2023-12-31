#include "TextureManager.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
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
	thisFrameLoadFlg_(false),
	threadTextureBuff_(),
	load_(),
	isThreadFinish_(false),
	fence_(),
	fenceVal_(0),
	fenceEvent_(nullptr),
	srvHeap_(nullptr),
	isCloaseCommandList_(false),
	isNowThreadLoading_(false)
{
	ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();

	// コマンドキューを作成
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<TextureManager>("CreateCommandQueue() Failed", "Constructor");
	}

	// コマンドアロケータを生成する
	commandAllocator_ = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<TextureManager>("CreateCommandAllocator() Failed", "Constructor");
	}

	// コマンドリストを作成する
	commandList_ = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<TextureManager>("CreateCommandList() Failed", "Constructor");
	}

	// 初期値0でFenceを作る
	fence_ = nullptr;
	fenceVal_ = 0;
	hr = device->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<TextureManager>("CreateFence() Failed", "Constructor");
	}

	// FenceのSignalを持つためのイベントを作成する
	fenceEvent_ = nullptr;
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
	if (!(fenceEvent_ != nullptr)) {
		throw Lamb::Error::Code<TextureManager>("CreateEvent() Failed", "Constructor");
	}

	srvHeap_ = CbvSrvUavHeap::GetInstance();

	Lamb::AddLog("Initialize TextureManager succeeded");
}

TextureManager::~TextureManager() {
	if (load_.joinable()) {
		load_.join();
	}
	CloseHandle(fenceEvent_);
	textures_.clear();
}


Texture* const TextureManager::LoadTexture(const std::string& fileName) {
	if (!std::filesystem::exists(fileName)) {
		throw Lamb::Error::Code<TextureManager>("this file is not exist -> " + fileName, __func__);
	}

	auto itr = textures_.find(fileName);
	if (itr == textures_.end()) {
		auto tex = std::make_unique<Texture>();
		tex->Load(fileName, commandList_.Get());

		/// コマンドリスト
		commandList_->Close();
		ID3D12CommandList* commandLists[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(_countof(commandLists), commandLists);
		// Fenceの値を更新
		fenceVal_++;
		// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
		commandQueue_->Signal(fence_.Get(), fenceVal_);

		// Fenceの値が指定したSigna値にたどり着いているか確認する
		// GetCompletedValueの初期値はFence作成時に渡した初期値
		if (fence_->GetCompletedValue() < fenceVal_) {
			// 指定したSignal値にたどり着いていないので、たどり着くまで待つようにイベントを設定する
			fence_->SetEventOnCompletion(fenceVal_, fenceEvent_);
			// イベントを待つ
			WaitForSingleObject(fenceEvent_, INFINITE);
		}

		// 次フレーム用のコマンドリストを準備
		auto hr = commandAllocator_->Reset();
		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			throw Lamb::Error::Code<TextureManager>("CommandAllocator somthing error", __func__);
		}
		hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			throw Lamb::Error::Code<TextureManager>("CommandList somthing error", __func__);
		}
		///

		if (!tex->isLoad_) {
			return nullptr;
		}

		srvHeap_->CreateView(*tex);

		textures_.insert(std::make_pair(fileName, std::move(tex)));

		thisFrameLoadFlg_ = true;
	}

	return textures_[fileName].get();
}

Texture* const TextureManager::LoadTexture(const std::string& fileName, ID3D12GraphicsCommandList* const commandList) {
	if (!std::filesystem::exists(fileName)) {
		throw Lamb::Error::Code<TextureManager>("this file is not exist -> " + fileName, __func__);
	}
	auto itr = textures_.find(fileName);
	if (itr == textures_.end()) {
		auto tex = std::make_unique<Texture>();
		tex->Load(fileName, commandList);
		if (!tex->isLoad_) {
			return nullptr;
		}

		srvHeap_->CreateView(*tex);

		textures_.insert(std::make_pair(fileName, std::move(tex)));

		thisFrameLoadFlg_ = true;
	}

	return textures_[fileName].get();
}

void TextureManager::LoadTexture(const std::string& fileName, Texture** const texPtr) {
	// コンテナに追加
	threadTextureBuff_.push({ fileName, texPtr });
}

void TextureManager::ThreadLoadTexture() {
	if (!load_.joinable() && !threadTextureBuff_.empty()) {
		auto loadProc = [this]() {
			isNowThreadLoading_ = true;
			while (!threadTextureBuff_.empty()) {
				if (Lamb::IsEngineFianlize()) {
					break;
				}
				auto& front = threadTextureBuff_.front();
				*front.second = LoadTexture(front.first, commandList_.Get());
				threadTextureBuff_.pop();
			}

			isThreadFinish_ = true;
			isNowThreadLoading_ = false;
		};

		load_ = std::thread(loadProc);
	}
}

Texture* const TextureManager::GetWhiteTex() {
	return instance_->textures_["./Resources/white2x2.png"].get();
}

void TextureManager::ReleaseIntermediateResource() {
	if (thisFrameLoadFlg_ && threadTextureBuff_.empty()) {
		for (auto& i : textures_) {
			i.second->ReleaseIntermediateResource();
		}

		thisFrameLoadFlg_ = false;
	}
}

void TextureManager::ResetCommandList() {
	if (isThreadFinish_) {
		isCloaseCommandList_ = true;
		commandList_->Close();
		ID3D12CommandList* commandLists[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(_countof(commandLists), commandLists);
		// Fenceの値を更新
		fenceVal_++;
		// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
		commandQueue_->Signal(fence_.Get(), fenceVal_);

		// Fenceの値が指定したSigna値にたどり着いているか確認する
		// GetCompletedValueの初期値はFence作成時に渡した初期値
		if (fence_->GetCompletedValue() < fenceVal_) {
			// 指定したSignal値にたどり着いていないので、たどり着くまで待つようにイベントを設定する
			fence_->SetEventOnCompletion(fenceVal_, fenceEvent_);
			// イベントを待つ
			WaitForSingleObject(fenceEvent_, INFINITE);
		}

		if (load_.joinable()) {
			load_.join();
		}

		// 次フレーム用のコマンドリストを準備
		auto hr = commandAllocator_->Reset();
		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			throw Lamb::Error::Code<TextureManager>("CommandAllocator somthing error", __func__);
		}
		hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			throw Lamb::Error::Code<TextureManager>("CommandList somthing error", __func__);
		}

		isThreadFinish_ = false;
		isCloaseCommandList_ = false;
	}
}

void TextureManager::Use(uint32_t texIndex, UINT rootParam) {
	auto* const mainComlist = DirectXCommand::GetInstance()->GetCommandList();
	mainComlist->SetGraphicsRootDescriptorTable(
		rootParam, srvHeap_->GetGpuHeapHandle(texIndex));
}