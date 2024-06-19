#pragma once

#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DescriptorHeap/Descriptor.h"

#include "Engine/EngineUtils/LambPtr/LambPtr.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Utils/Cocepts/Cocepts.h"

#include <array>

template<class T, size_t kBufferSize>
class RWStructuredBuffer : public Descriptor {
public:
	using type = T;

public:
	RWStructuredBuffer() :
		data_(),
		bufferResource_(),
		uavDesc_({}),
		buffer_({})
	{
		bufferResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(
			sizeof(type) * kBufferSize
		);




		uavDesc_ = {};

		uavDesc_.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc_.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc_.Buffer.FirstElement = 0;
		uavDesc_.Buffer.NumElements = UINT(kBufferSize);
		uavDesc_.Buffer.CounterOffsetInBytes = 0;
		uavDesc_.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		uavDesc_.Buffer.StructureByteStride = sizeof(T);

	}

	RWStructuredBuffer(const RWStructuredBuffer&) = delete;
	RWStructuredBuffer(RWStructuredBuffer&&) = delete;

	RWStructuredBuffer& operator=(const RWStructuredBuffer&) = delete;
	RWStructuredBuffer& operator=(RWStructuredBuffer&&) = delete;
public:
	~RWStructuredBuffer() = default;

public:
	void CreateView(
		D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCPU,
		D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU,
		UINT heapHandle) noexcept
	{
		heapHandleCPU_ = heapHandleCPU;
		heapHandleGPU_ = heapHandleGPU;
		heapHandle_ = heapHandle;

		Lamb::SafePtr device = DirectXDevice::GetInstance()->GetDevice();
		device->CreateUnorderedAccessView(bufferResource_.Get(), nullptr, &uavDesc_, heapHandleCPU_);

		isCreateView_ = true;
	}

public:
	void OnWright() noexcept {
		if (!isWright_) {
			bufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
			isWright_ = !isWright_;
		}
	}

	void OffWright() noexcept {
		if (isWright_) {
			bufferResource_->Unmap(0, nullptr);
			isWright_ = !isWright_;
		}
	}

	template<Lamb::IsInt IsInt>
	T& operator[](IsInt index) {
		return data_[index];
	}

	template<Lamb::IsInt IsInt>
	const T& operator[](IsInt index) const {
		return data_[index];
	}


public:
	constexpr size_t size() const noexcept {
		return kBufferSize;
	}

private:
	Lamb::SafePtr<type> data_;

	Lamb::LambPtr<ID3D12Resource> bufferResource_;
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc_;

	bool isCreateView_;
};