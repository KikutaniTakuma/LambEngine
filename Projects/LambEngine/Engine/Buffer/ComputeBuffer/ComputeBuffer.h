#pragma once

#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DescriptorHeap/Descriptor.h"

#include "Engine/EngineUtils/LambPtr/LambPtr.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Utils/Cocepts/Cocepts.h"

#include <array>

template<class T, size_t bufferSize>
class ComputeBuffer : public Descriptor {
public:
	using type = T;

public:
	ComputeBuffer() :
		data_(),
		bufferResource_(),
		uavDesc_({}),
		buffer_({})
	{
		// バイトサイズは256アライメントする(vramを効率的に使うための仕組み)
		size_t resourceSize = (sizeof(T) * bufferSize + 0xff) & ~0xff;

		D3D12_HEAP_PROPERTIES prop{};
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		prop.CreationNodeMask = 1;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		prop.Type = D3D12_HEAP_TYPE_CUSTOM;
		prop.VisibleNodeMask = 1;
		//サイズは定数バッファと同じように指定
		D3D12_RESOURCE_DESC desc{};
		desc.Alignment = 0;
		desc.DepthOrArraySize = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Height = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.MipLevels = 1;
		desc.SampleDesc = { 1, 0 };
		desc.Width = resourceSize;

		Lamb::SafePtr device = DirectXDevice::GetInstance()->GetDevice();

		HRESULT hr = device->CreateCommittedResource(
			&prop, D3D12_HEAP_FLAG_NONE, &desc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr,
			IID_PPV_ARGS(bufferResource_.GetAddressOf())
		);
		if (not SUCCEEDED(hr)) {
			throw Lamb::Error::Code<ComputeBuffer>("CreateCommittedResource failed", __func__);
		}
#ifdef _DEBUG
		bufferResource_.SetName<ComputeBuffer>();
#endif // _DEBUG

		D3D12_RANGE range{ 0, 1 };
		bufferResource_->Map(0, &range, reinterpret_cast<void**>(&data_));
		
		uavDesc_.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc_.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc_.Buffer.NumElements = bufferSize;
		uavDesc_.Buffer.StructureByteStride = sizeof(T);

	}

	ComputeBuffer(const ComputeBuffer&) = delete;
	ComputeBuffer(ComputeBuffer&&) = delete;

	ComputeBuffer& operator=(const ComputeBuffer&) = delete;
	ComputeBuffer& operator=(ComputeBuffer&&) = delete;
public:
	~ComputeBuffer() = default;

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
	template<Lamb::IsInt IsInt>
	T& operator[](IsInt index) {
		return buffer_[index];
	}

	template<Lamb::IsInt IsInt>
	const T& operator[](IsInt index) const {
		return buffer_[index];
	}


public:
	constexpr size_t size() const noexcept {
		return bufferSize;
	}

private:
	Lamb::SafePtr<T> data_;

	Lamb::LambPtr<ID3D12Resource> bufferResource_;
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc_;

	std::array<T, bufferSize> buffer_;

	bool isCreateView_;
};