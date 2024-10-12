#pragma once
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Utils/ExecutionLog.h"
#include "Utils/Concepts.h"
#include "Error/Error.h"
#include <cassert>
#include "Engine/Core/DescriptorHeap/Descriptor.h"
#include "Engine/Core/DescriptorHeap/DescriptorHeap.h"
#include "Utils/SafePtr.h"

/// <summary>
/// ストラクチャードバッファ
/// </summary>
/// <typeparam name="ValueType">ポインタと参照型以外をサポート</typeparam>
template<Lamb::IsNotReferenceAndPtr ValueType>
class StructuredBuffer final : public Descriptor {
public:
	using value_type = ValueType;
	using reference_type = value_type&;
	using const_reference_type = const value_type&;

	using pointer = value_type*;
	using const_pointer = const value_type*;


public:
	StructuredBuffer() :
		bufferResource_(),
		srvDesc_(),
		data_(nullptr),
		isWright_(false),
		bufferSize_(0u),
		isCreateView_(false)
	{}

	~StructuredBuffer() = default;

	// コピーやムーブはしないので削除
private:
	StructuredBuffer(const StructuredBuffer&) = delete;
	StructuredBuffer(StructuredBuffer&&) = delete;

	StructuredBuffer& operator=(const StructuredBuffer&) = delete;
	StructuredBuffer& operator=(StructuredBuffer&&) = delete;

public:
	void Create(uint32_t bufferSize) {
		bufferSize_ = bufferSize;

		bufferResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(ValueType) * size());
#ifdef USE_DEBUG_CODE
		bufferResource_.SetName<decltype(*this)>();
#endif // USE_DEBUG_CODE
		srvDesc_ = {};
		srvDesc_.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc_.Buffer.FirstElement = 0;
		srvDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc_.Buffer.NumElements = size();
		srvDesc_.Buffer.StructureByteStride = sizeof(ValueType);
	}

	void Reset(DescriptorHeap* descriptorHeap) {
		descriptorHeap->ReleaseView(GetHandleUINT());

		bufferResource_.Reset();
		srvDesc_.Buffer.NumElements = 0;
	}

	void MemCpy(const void* pSrc, size_t size) {
		OnWright();
		std::memcpy(data_, pSrc, size);
		OffWright();
	}

public:
	void OnWright() noexcept {
		if (not isWright_) {
			bufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
			isWright_ = true;
		}
	}

	void OffWright() noexcept {
		if (isWright_) {
			bufferResource_->Unmap(0, nullptr);
			isWright_ = false;
		}
	}

	template<Lamb::IsInt IsInt>
	reference_type operator[](IsInt index) {
		if (bufferSize_ <= static_cast<uint32_t>(index) or not isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Out of array references or did not Map");
#else
			throw Lamb::Error::Code<StructuredBuffer>("Out of array references or did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}
		return data_[index];
	}

	template<Lamb::IsInt IsInt>
	const_reference_type operator[](IsInt index) const {
		if (bufferSize_ <= static_cast<uint32_t>(index) or not isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Out of array references or did not Map");
#else
			throw Lamb::Error::Code<StructuredBuffer>("Out of array references or did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}
		return data_[index];
	}

	uint32_t size() const {
		return bufferSize_;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVtlAdrs() const noexcept {
		return bufferResource_->GetGPUVirtualAddress();
	}

	void CreateView(
		D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCPU,
		D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU,
		UINT heapHandle) noexcept
	{
		Lamb::SafePtr device = DirectXDevice::GetInstance()->GetDevice();
		device->CreateShaderResourceView(bufferResource_.Get(), &srvDesc_, heapHandleCPU);
		heapHandleCPU_ = heapHandleCPU;
		heapHandleGPU_ = heapHandleGPU;
		heapHandle_ = heapHandle;

		isCreateView_ = true;
	}

private:
	Lamb::LambPtr<ID3D12Resource> bufferResource_;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_;

	ValueType* data_;

	uint32_t bufferSize_;

	bool isWright_;
	bool isCreateView_;
};