#pragma once
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Utils/ExecutionLog.h"
#include "Utils/Concepts.h"
#include "Error/Error.h"
#include <cassert>
#include "Engine/Core/DescriptorHeap/Descriptor.h"
#include "Utils/SafePtr.h"

/// <summary>
/// ストラクチャードバッファ
/// </summary>
/// <typeparam name="T">ポインタと参照型以外をサポート</typeparam>
template<Lamb::IsNotReferenceAndPtr T>
class StructuredBuffer final : public Descriptor {
public:
	StructuredBuffer() :
		bufferResource_(),
		srvDesc_(),
		data_(nullptr),
		isWright_(true),
		bufferSize_(0u),
		isCreateView_(false)
	{
		
	}

	~StructuredBuffer() = default;

	inline StructuredBuffer(const StructuredBuffer& right) :
		StructuredBuffer()
	{
		*this = right;
	}

	inline StructuredBuffer(StructuredBuffer&& right) :
		StructuredBuffer()
	{
		*this = right;
	}

	inline StructuredBuffer& operator=(const StructuredBuffer& right) {
		bufferResource_ = right.bufferResource_;
		srvDesc_ = right.srvDesc_;
		isWright_ = right.isWright_;
		bufferSize_ = right.bufferSize_;
		isCreateView_ = right.isCreateView_;

		data_ = right.data_;

		return *this;
	}
	inline StructuredBuffer<T>& operator=(StructuredBuffer&& right) noexcept {
		bufferResource_ = right.bufferResource_.Release();
		srvDesc_ = right.srvDesc_;
		isWright_ = right.isWright_;
		bufferSize_ = right.bufferSize_;
		isCreateView_ = right.isCreateView_;

		data_ = right.data_;

		return *this;
	}

public:
	void Create(uint32_t bufferSize) {
		bufferSize_ = bufferSize;

		bufferResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(T) * size());
#ifdef _DEBUG
		bufferResource_.SetName<decltype(*this)>();
#endif // _DEBUG
		srvDesc_ = {};
		srvDesc_.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc_.Buffer.FirstElement = 0;
		srvDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc_.Buffer.NumElements = size();
		srvDesc_.Buffer.StructureByteStride = sizeof(T);

		if (isWright_) {
			bufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		}
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
		assert(static_cast<uint32_t>(index) < bufferSize_);
		return data_[index];
	}

	template<Lamb::IsInt IsInt>
	const T& operator[](IsInt index) const {
		assert(static_cast<uint32_t>(index) < bufferSize_);
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

	T* data_;

	uint32_t bufferSize_;

	bool isWright_;
	bool isCreateView_;
};