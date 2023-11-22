#include "DirectXDevice.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/EngineUtils/ErrorCheck/ErrorCheck.h"
#include "Utils/ConvertString/ConvertString.h"
#include <cassert>
#include <format>

DirectXDevice* DirectXDevice::instance_ = nullptr;

DirectXDevice* DirectXDevice::GetInstance() {
	return instance_;
}

void DirectXDevice::Initialize() {
	instance_ = new DirectXDevice{};
}
void DirectXDevice::Finalize() {
	delete instance_;
	instance_ = nullptr;
}


DirectXDevice::DirectXDevice():
	incrementSRVCBVUAVHeap_(0u),
	incrementRTVHeap_(0u),
	incrementDSVHeap_(0u),
	incrementSAMPLER_(0u),
	device_{},
	dxgiFactory_{},
	useAdapter_{}
{
	// IDXGIFactory生成
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect3D() : CreateDXGIFactory() Failed", "Engine");
		return;
	}

	// 使用するグラボの設定
	SettingAdapter();
	if (useAdapter_ == nullptr) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect3D() : GPU not Found", "Engine");
		return;
	}

	// Deviceの初期化
	// 使用しているデバイスによってD3D_FEATURE_LEVELの対応バージョンが違うので成功するまでバージョンを変えて繰り返す
	CreateDevice();

	if (device_ == nullptr) {
		return;
	}

#ifdef _DEBUG
	InfoQueue();
#endif

	CreateHeapIncrements();
}

void DirectXDevice::SettingAdapter() {
	useAdapter_ = nullptr;
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(useAdapter_.GetAddressOf())) != DXGI_ERROR_NOT_FOUND;
		++i) {

		DXGI_ADAPTER_DESC3 adapterDesc{};
		HRESULT hr = useAdapter_->GetDesc3(&adapterDesc);
		if (hr != S_OK) {
			ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect3D() : GetDesc3() Failed", "Engine");
			return;
		}

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Log::AddLog(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter_.Reset();
	}
}

void DirectXDevice::CreateDevice() {
	// Deviceの初期化
	// 使用しているデバイスによってD3D_FEATURE_LEVELの対応バージョンが違うので成功するまでバージョンを変えて繰り返す
	std::array featureLevels = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
	};
	std::array featureLevelString = {
		"12.2", "12.1", "12.0"
	};

	for (size_t i = 0; i < featureLevels.size(); ++i) {
		HRESULT hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(device_.GetAddressOf()));

		if (SUCCEEDED(hr)) {
			Log::AddLog(std::format("FeatureLevel:{}\n", featureLevelString[i]));
			break;
		}
	}

	if (device_ == nullptr) {
		return;
	}
	Log::AddLog("Complete create D3D12Device!!!\n");
}

#ifdef _DEBUG
void DirectXDevice::InfoQueue() const {
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラーの予期に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		// 解放
		infoQueue->Release();
	}
}
#endif // _DEBUG

void DirectXDevice::CreateHeapIncrements() {
	incrementSRVCBVUAVHeap_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	incrementRTVHeap_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	incrementDSVHeap_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	incrementSAMPLER_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
}

[[nodiscard]]
ID3D12DescriptorHeap* DirectXDevice::CreateDescriptorHeap(
	D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderrVisible
) {
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderrVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (SUCCEEDED(device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap)))) {
		return descriptorHeap;
	}
	assert(!"Failed");
	ErrorCheck::GetInstance()->ErrorTextBox("CreateDescriptorHeap() Failed", "Engine");

	return nullptr;
}

[[nodiscard]]
ID3D12Resource* DirectXDevice::CreateBufferResuorce(size_t sizeInBytes) {
	if (!device_) {
		OutputDebugStringA("device is nullptr!!");
		return nullptr;
	}

	// Resourceを生成する
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapPropaerties{};
	uploadHeapPropaerties.Type = D3D12_HEAP_TYPE_UPLOAD;
	// リソースの設定
	D3D12_RESOURCE_DESC resouceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定にする
	resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resouceDesc.Width = sizeInBytes;
	// バッファの場合はこれにする決まり
	resouceDesc.Height = 1;
	resouceDesc.DepthOrArraySize = 1;
	resouceDesc.MipLevels = 1;
	resouceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	ID3D12Resource* resuorce = nullptr;
	resuorce = nullptr;
	HRESULT hr = device_->CreateCommittedResource(&uploadHeapPropaerties, D3D12_HEAP_FLAG_NONE, &resouceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resuorce));
	if (!SUCCEEDED(hr)) {
		OutputDebugStringA("CreateCommittedResource Function Failed!!");
		ErrorCheck::GetInstance()->ErrorTextBox("CreateBufferResuorce() : CreateCommittedResource() Failed", "Engine");
		return nullptr;
	}

	return resuorce;
}

[[nodiscard]]
ID3D12Resource* DirectXDevice::CreateDepthStencilTextureResource(int32_t width, int32_t height) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	ID3D12Resource* resource = nullptr;
	if (!SUCCEEDED(
		device_->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(&resource))
	)) {
		assert(!"CreateDepthStencilTextureResource Failed");
		ErrorCheck::GetInstance()->ErrorTextBox("CreateDepthStencilTextureResource() Failed", "Engine");
	}

	return resource;
}

[[nodiscard]]
ID3D12Resource* DirectXDevice::CreateDepthStencilTextureResource(const Vector2& size) {
	return CreateDepthStencilTextureResource(
		static_cast<int32_t>(size.x),
		static_cast<int32_t>(size.y)
	);
}
