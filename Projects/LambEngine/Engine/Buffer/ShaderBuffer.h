#pragma once
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXSwapChain/DirectXSwapChain.h"

#include "Utils/Concepts.h"
#include "Error/Error.h"

#include <cassert>

template<Lamb::IsNotReferenceAndPtr ValueType>
class ShaderBuffer {
public:
	using value_type = ValueType;

	using reference_type = value_type&;
	using const_reference_type = const value_type&;

	using pointer = value_type*;
	using const_pointer = const value_type*;

public:
	ShaderBuffer() = default;
	virtual ~ShaderBuffer() = default;

	// コピーやムーブはしないので削除
private:
	ShaderBuffer(const ShaderBuffer&) = delete;
	ShaderBuffer(ShaderBuffer&&) = delete;

	ShaderBuffer& operator=(const ShaderBuffer&) = delete;
	ShaderBuffer& operator=(ShaderBuffer&&) = delete;


public:
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVtlAdrs() const noexcept {
		return bufferResource_->GetGPUVirtualAddress();
	}

	uint32_t size() const noexcept {
		return bufferSize_;
	}

public:
	void OnWright() noexcept {
		if (not isWright_) {
			bufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&pData_));
			isWright_ = true;
		}
	}

	void OffWright() noexcept {
		if (isWright_) {
			bufferResource_->Unmap(0, nullptr);
			isWright_ = false;
		}
	}

	/// <summary>
	/// データのコピー
	/// </summary>
	/// <param name="pSrc">コピー元</param>
	/// <param name="size">コピーサイズ</param>
	void MemCpy(const void* pSrc, size_t size) {
		OnWright();
		std::memcpy(pData_, pSrc, size);
		OffWright();
	}

	/// <summary>
	/// 最初のデータへコピー
	/// </summary>
	/// <param name="pSrc">コピー元</param>
	void MemCpy(const void* pSrc) {
		OnWright();
		std::memcpy(pData_, pSrc, sizeof(value_type));
		OffWright();
	}

	template<Lamb::IsInt IsInt>
	reference_type operator[](IsInt index) {
		if (bufferSize_ <= static_cast<uint32_t>(index) or not isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Out of array references or did not Map");
#else
			throw Lamb::Error::Code<ShaderBuffer>("Out of array references or did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}

		return pData_[index];
	}

	template<Lamb::IsInt IsInt>
	const_reference_type operator[](IsInt index) const {
		if (bufferSize_ <= static_cast<uint32_t>(index) or not isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Out of array references or did not Map");
#else
			throw Lamb::Error::Code<ShaderBuffer>("Out of array references or did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}
		return pData_[index];
	}

	reference_type operator*() {
		if (not this->isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Did not Map");
#else
			throw Lamb::Error::Code<ShaderBuffer>("Did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}
		return *(this->pData_);
	}

	const_reference_type operator*() const {
		if (not this->isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Did not Map");
#else
			throw Lamb::Error::Code<ShaderBuffer>("Did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}
		return *(this->pData_);
	}

	pointer operator->() {
		if (not this->isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Did not Map");
#else
			throw Lamb::Error::Code<ShaderBuffer>("Did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}
		return (this->pData_);
	}

	const_pointer operator->() const {
		if (not this->isWright_) [[unlikely]] {
#ifdef USE_DEBUG_CODE
			assert(!"Did not Map");
#else
			throw Lamb::Error::Code<ShaderBuffer>("Did not Map", ErrorPlace);
#endif // USE_DEBUG_CODE
		}
		return (this->pData_);
	}

protected:
	Lamb::LambPtr<ID3D12Resource> bufferResource_;
	pointer pData_ = nullptr;
	uint32_t bufferSize_ = 0u;
	bool isWright_ = false;
};