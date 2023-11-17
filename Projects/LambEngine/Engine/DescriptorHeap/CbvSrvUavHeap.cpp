#include "CbvSrvUavHeap.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/EngineParts/DirectXDevice/DirectXDevice.h"
#include "Engine/EngineParts/DirectXCommon/DirectXCommon.h"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <numeric>

CbvSrvUavHeap* CbvSrvUavHeap::instance_ = nullptr;

void CbvSrvUavHeap::Initialize(UINT numDescriptor) {
	// 1～(10^6-1)でクランプ
	numDescriptor = std::clamp(numDescriptor, 1u, static_cast<UINT>(std::pow(10u, 6u)) - 1u);

	instance_ = new CbvSrvUavHeap{ numDescriptor };
}

void CbvSrvUavHeap::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

CbvSrvUavHeap* CbvSrvUavHeap::GetInstance() {
	return instance_;
}

CbvSrvUavHeap::CbvSrvUavHeap(UINT numDescriptor) :
	heap_(),
	heapSize_(numDescriptor),
	currentHandleIndex_(0u),
	heapHandles_(0),
	releaseHandle_(),
	useHandle_(),
	bookingHandle_()
{
	heap_ = DirectXDevice::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescriptor, true);

	UINT incrementSRVCBVUAVHeap = DirectXDevice::GetInstance()->GetIncrementSRVCBVUAVHeap();

	heapHandles_.reserve(heapSize_);
	heapHandles_.push_back({ heap_->GetCPUDescriptorHandleForHeapStart(),
							heap_->GetGPUDescriptorHandleForHeapStart() });
	auto heapHandleFirstItr = heapHandles_.begin();
	for (uint32_t i = 1; i < heapSize_; i++) {
		auto hadleTmp = *heapHandleFirstItr;
		hadleTmp.first.ptr += incrementSRVCBVUAVHeap * i;
		hadleTmp.second.ptr += incrementSRVCBVUAVHeap * i;
		heapHandles_.push_back(hadleTmp);
	}
	bookingHandle_.clear();
}

CbvSrvUavHeap::~CbvSrvUavHeap() {
	Reset();
}

void CbvSrvUavHeap::SetHeap() {
	static auto commandlist = DirectXCommon::GetInstance()->GetCommandList();
	commandlist->SetDescriptorHeaps(1, heap_.GetAddressOf());
}

void CbvSrvUavHeap::Use(D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT rootParmIndex) {
	static auto commandlist = DirectXCommon::GetInstance()->GetCommandList();
	commandlist->SetGraphicsRootDescriptorTable(rootParmIndex, handle);
}

void CbvSrvUavHeap::Use(uint32_t handleIndex, UINT rootParmIndex) {
	auto commandlist = DirectXCommon::GetInstance()->GetCommandList();
	commandlist->SetGraphicsRootDescriptorTable(rootParmIndex, heapHandles_[handleIndex].second);
}

void CbvSrvUavHeap::Reset() {
	if (heap_) {
		heap_->Release();
		heap_.Reset();
	}
}

uint32_t CbvSrvUavHeap::CreateTxtureView(Texture* tex) {
	assert(tex != nullptr);
	if (tex == nullptr || !*tex) {
		return currentHandleIndex_;
	}
	assert(currentHandleIndex_ < heapSize_);
	if (currentHandleIndex_ >= heapSize_) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateTxtureBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		return std::numeric_limits<uint32_t>::max();
	}

	if (bookingHandle_.empty()) {
		useHandle_.push_back(currentHandleIndex_);
		tex->CreateSRVView(
			heapHandles_[currentHandleIndex_].first,
			heapHandles_[currentHandleIndex_].second,
			currentHandleIndex_
			);
		currentHandleIndex_++;
		return currentHandleIndex_ - 1u;
	}
	// もしリリースした場所に作るなら
	else {
		uint32_t nowCreateViewHandle = bookingHandle_.front();
		useHandle_.push_back(nowCreateViewHandle);
		tex->CreateSRVView(
			heapHandles_[nowCreateViewHandle].first,
			heapHandles_[nowCreateViewHandle].second,
			nowCreateViewHandle
		);
		bookingHandle_.pop_front();
		return nowCreateViewHandle;
	}
}

void CbvSrvUavHeap::CreateTxtureView(Texture* tex, uint32_t heapIndex) {
	assert(tex != nullptr);
	assert(heapIndex < heapSize_);
	if (currentHandleIndex_ >= heapSize_) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreatTxtureBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		return;
	}

	tex->CreateSRVView(
		heapHandles_[heapIndex].first,
		heapHandles_[heapIndex].second,
		heapIndex
		);
}

uint32_t CbvSrvUavHeap::CreatePerarenderView(RenderTarget& renderTarget) {
	assert(currentHandleIndex_ < heapSize_);
	if (currentHandleIndex_ >= heapSize_) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreatePerarenderView failed\nOver HeapSize", "ShaderResourceHeap");
		return std::numeric_limits<uint32_t>::max();
	}

	if (bookingHandle_.empty()) {
		useHandle_.push_back(currentHandleIndex_);
		renderTarget.CreateView(heapHandles_[currentHandleIndex_].first, heapHandles_[currentHandleIndex_].second, currentHandleIndex_);
		currentHandleIndex_++;
		return currentHandleIndex_ - 1u;
	}
	else {
		uint32_t nowCreateViewHandle = bookingHandle_.front();
		useHandle_.push_back(nowCreateViewHandle);
		renderTarget.CreateView(heapHandles_[nowCreateViewHandle].first, heapHandles_[nowCreateViewHandle].second, nowCreateViewHandle);
		bookingHandle_.pop_front();
		return nowCreateViewHandle;
	}
}

uint32_t CbvSrvUavHeap::BookingHeapPos(UINT nextCreateViewNum) {
	// リリースハンドルがないなら予約しない
	if (releaseHandle_.empty()) {
		bookingHandle_.clear();
		return currentHandleIndex_;
	}
	// ディスクリプタが一つも作られてない場合は0を指定
	if (useHandle_.empty()) {
		currentHandleIndex_ = 0u;
		return currentHandleIndex_;
	}

	// リリースハンドルの最初のイテレータ
	auto releaseHandle = releaseHandle_.begin();
	// 予約済みを使えるかどうか
	bool isCanUseHandle = true;

	size_t loopCount = 0llu;

	do {
		releaseHandle = releaseHandle_.begin();
		size_t loopCountBuf = loopCount;
		while (0llu < loopCountBuf) {
			releaseHandle++;
			if (releaseHandle == releaseHandle_.end()) {
				return currentHandleIndex_;
			}
			loopCountBuf--;
		}

		// ループするのでここでも初期化
		isCanUseHandle = true;
		// 予約済みのハンドルをクリア
		bookingHandle_.clear();

		// 予約する数分格納
		if (nextCreateViewNum != 0u) {
			bookingHandle_.push_back(*releaseHandle);
		}
		for (uint32_t i = 1; i < nextCreateViewNum; i++) {
			releaseHandle++;
			// 格納中にリリースハンドルが最後まで行ってしまったら
			// 解放しループを抜ける
			if (releaseHandle == releaseHandle_.end()) {
				bookingHandle_.clear();
				break;
			}
			bookingHandle_.push_back(*releaseHandle);
		}

		// 予約出来なかったらループを抜けて早期リターン
		if (bookingHandle_.empty()) {
			return currentHandleIndex_;
		}

		// 予約したものが連続したハンドルか
		for (size_t i = 0; i < bookingHandle_.size() - 1; i++) {
			// 連続してない場合予約ハンドルを解放してループを抜ける
			if (bookingHandle_[i] != bookingHandle_[i + 1llu] - 1u) {
				isCanUseHandle = false;
				bookingHandle_.clear();
				break;
			}
		}

		loopCount++;
		// ちゃんと予約出来るまでループ
	} while (!isCanUseHandle);

	for (auto& i : bookingHandle_) {
		// 予約済みになったリリースハンドルを解放
		std::erase(releaseHandle_, i);
		// 予約済みになったUseハンドルを解放
		std::erase(useHandle_, i);
	}

	if (!bookingHandle_.empty()) {
		return bookingHandle_.front();
	}
	else {
		return currentHandleIndex_;
	}
}


void CbvSrvUavHeap::ReleaseView(UINT viewHandle) {
	if (!releaseHandle_.empty()) {
		// リリースハンドルにすでに格納されているか
		auto isReleased = std::find(releaseHandle_.begin(), releaseHandle_.end(), viewHandle);
		if (isReleased != releaseHandle_.end()) {
			// すでにリリースハンドルがある
			return;
		}
	}
	// Useハンドルの中に今から解放するハンドルがあるか
	auto isReleased = std::find(useHandle_.begin(), useHandle_.end(), viewHandle);
	if (isReleased == useHandle_.end()) {
		// そもそも解放するディスクリプタがないので早期リターン
		return;
	}

	// リリースハンドルに格納
	releaseHandle_.push_back(viewHandle);
}

void CbvSrvUavHeap::UseThisPosition(uint32_t handle) {
	// 使ったハンドルがcurrentHandleIndex_と一致していらcurrentHandleIndex_をインクリメント
	if (currentHandleIndex_ == handle) {
		currentHandleIndex_++;
	}
	// 空だったら代入
	if (useHandle_.empty()) {
		useHandle_.push_back(handle);
		return;
	}

	auto isUsed = std::find(useHandle_.begin(), useHandle_.end(), handle);

	if (isUsed != useHandle_.end()) {
		// すでに格納済み
		return;
	}
	else {
		useHandle_.push_back(handle);
	}

	// 予約済みハンドルに含まれていたら削除
	auto isBookingHandle = std::find(bookingHandle_.begin(), bookingHandle_.end(), handle);
	if (isBookingHandle != bookingHandle_.end()) {
		std::erase(bookingHandle_, handle);
	}
}