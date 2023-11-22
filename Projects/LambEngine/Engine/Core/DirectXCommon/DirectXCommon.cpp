#include "DirectXCommon.h"
#include "Engine/Engine.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DescriptorHeap/RTVHeap.h"
#include "Engine/EngineUtils/ErrorCheck/ErrorCheck.h"

#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"

#include <cassert>

#ifdef _DEBUG
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wPram, LPARAM lPram);
#endif // _DEBUG

DirectXCommon* DirectXCommon::instance_ = nullptr;

DirectXCommon* DirectXCommon::GetInstance() {
	assert(!!instance_);
	return instance_;
}

void DirectXCommon::Initialize() {
	assert(!instance_);
	instance_ = new DirectXCommon{};
	assert(!!instance_);
}
void DirectXCommon::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

DirectXCommon::DirectXCommon():
	swapChain_{},
	swapChainResource_{},
	isRenderState_{false}
{
	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();
	IDXGIFactory7* const dxgiFactory = DirectXDevice::GetInstance()->GetDxgiFactory();
	ID3D12CommandQueue* const commandQueue = DirectXCommand::GetInstance()->GetCommandQueue();

	// スワップチェーンの作成
	swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = 1280;
	swapChainDesc.Height = 720;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = kBackBufferNumber_;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, WindowFactory::GetInstance()->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateSwapChainForHwnd() Failed", "Engine");
		return;
	}

	dxgiFactory->MakeWindowAssociation(
		WindowFactory::GetInstance()->GetHwnd(), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

	RtvHeap* const rtvHeap = RtvHeap::GetInstance();
	rtvHeap->CreateBackBuffer(swapChainResource_, swapChain_.Get());

#ifdef _DEBUG
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// SRV用のヒープ
	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	uint32_t useHandle = descriptorHeap->BookingHeapPos(1u);

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WindowFactory::GetInstance()->GetHwnd());
	ImGui_ImplDX12_Init(
		device,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		descriptorHeap->Get(),
		descriptorHeap->GetCpuHeapHandle(useHandle),
		descriptorHeap->GetGpuHeapHandle(useHandle)
	);

	descriptorHeap->UseThisPosition(useHandle);
#endif // DEBUG
}

DirectXCommon::~DirectXCommon() {
#ifdef _DEBUG
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // _DEBUG
}

void DirectXCommon::SetViewPort(uint32_t width, uint32_t height) {
	ID3D12GraphicsCommandList* const commandList = DirectXCommand::GetInstance()->GetCommandList();

	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandList->RSSetViewports(1, &viewport);

	// シザー矩形
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ矩形が構成されるようになる
	scissorRect.left = 0;
	scissorRect.right = static_cast<LONG>(WindowFactory::GetInstance()->GetWindowSize().x);
	scissorRect.top = 0;
	scissorRect.bottom = static_cast<LONG>(WindowFactory::GetInstance()->GetWindowSize().y);
	commandList->RSSetScissorRects(1, &scissorRect);
}

void DirectXCommon::Barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, UINT subResource) {
	ID3D12GraphicsCommandList* const commandList = DirectXCommand::GetInstance()->GetCommandList();

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース
	barrier.Transition.pResource = resource;
	// subResourceの設定
	barrier.Transition.Subresource = subResource;
	// 遷移前(現在)のResouceState
	barrier.Transition.StateBefore = before;
	// 遷移後のResouceState
	barrier.Transition.StateAfter = after;
	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);
}

void DirectXCommon::SetMainRenderTarget() {
	RtvHeap* const rtvHeap = RtvHeap::GetInstance();
	rtvHeap->SetMainRtv();
}

void DirectXCommon::ClearBackBuffer() {
	ID3D12GraphicsCommandList* const commandList = DirectXCommand::GetInstance()->GetCommandList();


	auto dsvH = Engine::GetDsvHandle();
	commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// 指定した色で画面全体をクリアする
	RtvHeap* const rtvHeap = RtvHeap::GetInstance();
	Vector4 clearColor = { 0.1f, 0.25f, 0.5f, 0.0f };
	rtvHeap->ClearRenderTargetView(backBufferIndex, clearColor);
}

void DirectXCommon::ChangeBackBufferState() {
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	if (isRenderState_) {
		Barrier(
			swapChainResource_[backBufferIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		isRenderState_ = false;
	}
	else {
		Barrier(
			swapChainResource_[backBufferIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		isRenderState_ = true;
	}
}

void DirectXCommon::SwapChainPresent() {
	// GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);
}