#pragma once
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXSwapChain/DirectXSwapChain.h"
#include "Utils/Concepts.h"
#include "Engine/Core/DescriptorHeap/Descriptor.h"

/// <summary>
/// コンスタントバッファ
/// </summary>
/// <typeparam name="ValueType">ポインタと参照型以外をサポート</typeparam>
template<Lamb::IsNotReferenceAndPtr ValueType>
class ConstantBuffer final : public Descriptor {
public:
	using value_type = ValueType;
	using reference_type = value_type&;
	using const_reference_type = const value_type&;

	using pointer = value_type*;
	using const_pointer = const value_type*;

public:
	inline ConstantBuffer() :
		bufferResource_(),
		cbvDesc_(),
		data_(nullptr),
		isWright_(false),
		isCreateView_(false),
		roootParamater_(),
		shaderVisibility_(D3D12_SHADER_VISIBILITY_ALL),
		shaderRegister_(0)
	{
		// バイトサイズは256アライメントする(vramを効率的に使うための仕組み)
		bufferResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce((sizeof(ValueType) + 0xff) & ~0xff);
#ifdef USE_DEBUG_CODE
		bufferResource_.SetName<ConstantBuffer>();
#endif // USE_DEBUG_CODE

		cbvDesc_.BufferLocation = bufferResource_->GetGPUVirtualAddress();
		cbvDesc_.SizeInBytes = UINT(bufferResource_->GetDesc().Width);

		roootParamater_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	}

	~ConstantBuffer() = default;

// コピーやムーブはしないので削除
private:
	ConstantBuffer(const ConstantBuffer&) = delete;
	ConstantBuffer(ConstantBuffer&&) noexcept = delete;

	ConstantBuffer& operator=(const ConstantBuffer&) = delete;
	ConstantBuffer& operator=(ConstantBuffer&&) = delete;

public:
	void OnWright() noexcept {
		if (not isWright_) {
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

	void MemCpy(const void* pSrc) {
		OnWright();
		std::memcpy(data_, pSrc, sizeof(ValueType));
		OffWright();
	}

	reference_type operator*() {
		if (not isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Did not Map");
#else
			throw Lamb::Error::Code<StructuredBuffer>("Did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}
		return *data_;
	}

	const_reference_type operator*() const {
		if (not isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Did not Map");
#else
			throw Lamb::Error::Code<StructuredBuffer>("Did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}
		return *data_;
	}

	pointer operator->() {
		if (not isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Did not Map");
#else
			throw Lamb::Error::Code<StructuredBuffer>("Did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}
		return data_;
	}

	const_pointer operator->() const {
		if (not isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Did not Map");
#else
			throw Lamb::Error::Code<StructuredBuffer>("Did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}
		return data_;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVtlAdrs() const noexcept {
		return bufferResource_->GetGPUVirtualAddress();
	}

	const D3D12_ROOT_PARAMETER& GetRoootParamater() noexcept {
		roootParamater_.ShaderVisibility = shaderVisibility_;
		roootParamater_.Descriptor.ShaderRegister = shaderRegister_;
		return roootParamater_;
	}

	void CreateView(
		D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCPU,
		D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU,
		UINT heapHandle
	) noexcept {
		Lamb::SafePtr device = DirectXDevice::GetInstance()->GetDevice();
		device->CreateConstantBufferView(&cbvDesc_, heapHandleCPU);
		heapHandleCPU_ = heapHandleCPU;
		heapHandleGPU_ = heapHandleGPU;
		heapHandle_ = heapHandle;
		isCreateView_ = true;
	}

private:
	Lamb::LambPtr<ID3D12Resource> bufferResource_;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc_;

	ValueType* data_;

	bool isWright_;

	bool isCreateView_;

	D3D12_ROOT_PARAMETER roootParamater_;
public:
	D3D12_SHADER_VISIBILITY shaderVisibility_;
	UINT shaderRegister_;
};