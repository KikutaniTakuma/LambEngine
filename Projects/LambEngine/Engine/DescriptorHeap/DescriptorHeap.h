#pragma once

#include "Engine/EngineParts/DirectXDevice/DirectXDevice.h"
#include "Engine/EngineParts/DirectXCommon/DirectXCommon.h"
#include <wrl.h>
#include <list>
#include <deque>

class DescriptorHeap {
protected:
	DescriptorHeap();
	DescriptorHeap(const DescriptorHeap&) = delete;
	DescriptorHeap(DescriptorHeap&&) = delete;
	virtual ~DescriptorHeap();

	DescriptorHeap& operator=(const DescriptorHeap&) = delete;
	DescriptorHeap& operator=(DescriptorHeap&&) = delete;

/// <summary>
/// ディスクリプタヒープ生成関数
/// </summary>
protected:
	virtual void CreateDescriptorHeap(uint32_t heapSize) = 0;

public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHeapHandle(uint32_t heapIndex) {
		return heapHandles_[heapIndex].first;
	}
	virtual D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHeapHandle(uint32_t heapIndex) {
		return heapHandles_[heapIndex].second;
	}

	inline UINT GetSize() const {
		return heapSize_;
	}

	inline ID3D12DescriptorHeap* const Get() const {
		return heap_.Get();
	}

	inline ID3D12DescriptorHeap* const* const GetAddressOf() const {
		return heap_.GetAddressOf();
	}

public:
	uint32_t BookingHeapPos(UINT nextCreateViewNum);
	void ReleaseView(UINT viewHandle);

	/// <summary>
	/// Useハンドルコンテナに追加(既に追加済みなら追加されない)
	/// </summary>
	/// <param name=""></param>
	void UseThisPosition(uint32_t handle);

protected:
	virtual void CreateHeapHandles();

	void Reset();

protected:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;

	UINT heapSize_;
	UINT currentHandleIndex_;

	std::list<uint32_t> releaseHandle_;
	std::list<uint32_t> useHandle_;
	std::deque<uint32_t> bookingHandle_;


	std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>> heapHandles_;
};