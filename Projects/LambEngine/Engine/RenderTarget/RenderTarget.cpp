#include "RenderTarget.h"
#include "Engine/Engine.h"
#include "Engine/EngineParts/DirectXDevice/DirectXDevice.h"
#include "Engine/EngineParts/DirectXCommon/DirectXCommon.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include <cassert>
#include "Utils/Math/Vector4.h"

RenderTarget::RenderTarget():
	resource_(),
	RTVHeap_(),
	srvHeapHandle_(),
	isResourceStateChange_(false),
	width_(Engine::GetInstance()->clientWidth),
	height_(Engine::GetInstance()->clientHeight),
	srvDesc_{},
	srvHeapHandleUint_()
{
	auto resDesc = DirectXCommon::GetInstance()->GetSwapchainBufferDesc();

	// Resourceを生成する
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapPropaerties{};
	heapPropaerties.Type = D3D12_HEAP_TYPE_DEFAULT;
	Vector4 clsValue = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	for (size_t i = 0; i < clsValue.m.size(); i++) {
		clearValue.Color[i] = clsValue[i];
	}

	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();

	// 実際にリソースを作る
	HRESULT hr = device->
		CreateCommittedResource(
			&heapPropaerties, 
			D3D12_HEAP_FLAG_NONE, 
			&resDesc, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
			&clearValue, 
			IID_PPV_ARGS(resource_.GetAddressOf())
		);
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateCommittedResource Function Failed", "RenderTarget");
		return;
	}

	RTVHeap_ = DirectXDevice::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	device->
		CreateRenderTargetView(
			resource_.Get(),
			&rtvDesc,
			RTVHeap_->GetCPUDescriptorHandleForHeapStart()
		);

	srvDesc_ = {};
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc_.Format = rtvDesc.Format;
	srvDesc_.Texture2D.MipLevels = 1;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

RenderTarget::RenderTarget(uint32_t width, uint32_t height) :
	resource_(),
	RTVHeap_(),
	srvHeapHandle_(),
	isResourceStateChange_(false),
	width_(width),
	height_(height),
	srvDesc_{},
	srvHeapHandleUint_()
{
	auto resDesc = DirectXCommon::GetInstance()->GetSwapchainBufferDesc();
	resDesc.Width = width_;
	resDesc.Height = height_;


	// Resourceを生成する
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapPropaerties{};
	heapPropaerties.Type = D3D12_HEAP_TYPE_DEFAULT;
	Vector4 clsValue = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	for (size_t i = 0; i < clsValue.m.size(); i++) {
		clearValue.Color[i] = clsValue[i];
	}

	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();

	// 実際にリソースを作る
	HRESULT hr = device->
		CreateCommittedResource(
			&heapPropaerties,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(resource_.GetAddressOf())
		);
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateCommittedResource Function Failed", "RenderTarget");
		return;
	}

	RTVHeap_ = DirectXDevice::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	device->
		CreateRenderTargetView(
			resource_.Get(),
			&rtvDesc,
			RTVHeap_->GetCPUDescriptorHandleForHeapStart()
		);

	srvDesc_ = {};
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc_.Format = rtvDesc.Format;
	srvDesc_.Texture2D.MipLevels = 1;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

RenderTarget::~RenderTarget() {
	if (RTVHeap_) {
		RTVHeap_->Release();
	}
}

void RenderTarget::SetThisRenderTarget() {
	isResourceStateChange_ = false;

	DirectXCommon::GetInstance()->Barrier(
		resource_.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	auto rtvHeapHandle = RTVHeap_->GetCPUDescriptorHandleForHeapStart();
	auto dsvH = Engine::GetDsvHandle();
	DirectXCommon::GetInstance()->GetCommandList()->OMSetRenderTargets(1, &rtvHeapHandle, false, &dsvH);

	Vector4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectXCommon::GetInstance()->GetCommandList()->ClearRenderTargetView(rtvHeapHandle, clearColor.m.data(), 0, nullptr);
	//Engine::SetViewPort(width, height);
}

void RenderTarget::ChangeResourceState() {
	if (!isResourceStateChange_) {
		DirectXCommon::GetInstance()->Barrier(
			resource_.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
		isResourceStateChange_ = true;
	}
}

void RenderTarget::SetMainRenderTarget() {
	ChangeResourceState();
	
	DirectXCommon::GetInstance()->SetMainRenderTarget();
}

void RenderTarget::UseThisRenderTargetShaderResource() {
	static auto mainComList = DirectXCommon::GetInstance()->GetCommandList();
	mainComList->SetGraphicsRootDescriptorTable(0, srvHeapHandle_);
}

void RenderTarget::CreateView(D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle, D3D12_GPU_DESCRIPTOR_HANDLE descHeapHandleGPU, UINT descHeapHandleUINT) {
	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();
	
	device->CreateShaderResourceView(
		resource_.Get(),
		&srvDesc_,
		descHeapHandle
	);

	srvHeapHandle_ = descHeapHandleGPU;
	srvHeapHandleUint_ = descHeapHandleUINT;

	tex_.reset();
	tex_ = std::make_unique<Texture>();
	assert(tex_);
	tex_->Set(
		resource_,
		srvDesc_,
		srvHeapHandle_,
		srvHeapHandleUint_
	);
}