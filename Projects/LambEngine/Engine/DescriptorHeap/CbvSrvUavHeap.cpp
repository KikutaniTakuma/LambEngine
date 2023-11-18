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

void CbvSrvUavHeap::Initialize(UINT heapSize) {
	instance_ = new CbvSrvUavHeap{ heapSize };
}

void CbvSrvUavHeap::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

CbvSrvUavHeap* CbvSrvUavHeap::GetInstance() {
	return instance_;
}

CbvSrvUavHeap::CbvSrvUavHeap(UINT numDescriptor) :
	DescriptorHeap{}
{
	CreateDescriptorHeap(numDescriptor);

	CreateHeapHandles();

	bookingHandle_.clear();
}

CbvSrvUavHeap::~CbvSrvUavHeap() {
	Reset();
}

void CbvSrvUavHeap::CreateDescriptorHeap(uint32_t heapSize) {
	// 1～(10^6-1)でクランプ
	heapSize_ = std::clamp(heapSize, 1u, static_cast<UINT>(std::pow(10u, 6u)) - 1u);
	heap_ = DirectXDevice::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, heapSize_, true);
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