#pragma once

#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DescriptorHeap/Descriptor.h"

#include "Engine/EngineUtils/LambPtr/LambPtr.h"
#include "Utils/SafePtr.h"
#include "Utils/Concepts.h"

template<class T>
class RWStructuredBuffer : public Descriptor {
public:
	using type = T;

public:
	RWStructuredBuffer() :
		data_(),
		bufferResource_(),
		uavDesc_{},
		isCreateView_(false),
		isWright_(false),
		bufferSize_(1)
	{
	}

	RWStructuredBuffer(const RWStructuredBuffer&) = delete;
	RWStructuredBuffer(RWStructuredBuffer&&) = delete;

	RWStructuredBuffer& operator=(const RWStructuredBuffer&) = delete;
	RWStructuredBuffer& operator=(RWStructuredBuffer&&) = delete;
public:
	~RWStructuredBuffer() = default;

public:
	void Create(uint32_t bufferSize) {
		bufferSize_ = bufferSize;

		bufferResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(type) * size());
#ifdef _DEBUG
		bufferResource_.SetName<decltype(*this)>();
#endif // _DEBUG


		OnWright();


		uavDesc_ = {};

		uavDesc_.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc_.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc_.Buffer.FirstElement = 0;
		uavDesc_.Buffer.NumElements = bufferSize_;
		uavDesc_.Buffer.CounterOffsetInBytes = 0;
		uavDesc_.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		uavDesc_.Buffer.StructureByteStride = sizeof(T);
	}

	void CreateView(
		D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCPU,
		D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU,
		UINT heapHandle) noexcept override
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
			bufferResource_->Map(0, nullptr, data_.GetPtrAdress());
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
	uint32_t size() const noexcept {
		return bufferSize_;
	}

private:
	Lamb::SafePtr<type> data_;
	uint32_t bufferSize_;

	Lamb::LambPtr<ID3D12Resource> bufferResource_;
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc_;

	bool isCreateView_;
	bool isWright_;

};