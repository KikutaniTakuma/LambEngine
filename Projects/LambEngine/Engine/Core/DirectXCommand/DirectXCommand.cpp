#include "DirectXCommand.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/EngineUtils/ErrorCheck/ErrorCheck.h"
#include <cassert>

DirectXCommand* DirectXCommand::instance_ = nullptr;

DirectXCommand* const DirectXCommand::GetInstance() {
	return instance_;
}

void DirectXCommand::Initialize() {
	assert(!instance_);
	instance_ = new DirectXCommand;
}
void DirectXCommand::Finalize() {
	delete instance_;
	instance_ = nullptr;
}


DirectXCommand::DirectXCommand():
	commandQueue_{},
	commandAllocator_{},
	commandList_{},
	isCommandListClose_{false},
	fence_{},
	fenceVal_{0llu},
	fenceEvent_{nullptr}
{
	CreateCommandQueue();

	CreateCommandAllocator();

	CreateGraphicsCommandList();

	CrateFence();
}

DirectXCommand::~DirectXCommand() {
	CloseHandle(fenceEvent_);
}

void DirectXCommand::CloseCommandlist() {
	// コマンドリストを確定させる
	HRESULT hr = commandList_->Close();
	isCommandListClose_ = true;
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CommandList->Close() Failed", "Engine");
	}
}

void DirectXCommand::ExecuteCommandLists() {
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void DirectXCommand::ResetCommandlist() {
	// 次フレーム用のコマンドリストを準備
	HRESULT hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CommandAllocator->Reset() Failed", "Engine");
		return;
	}
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CommandList->Reset() Failed", "Engine");
		return;
	}
	isCommandListClose_ = false;
}

void DirectXCommand::WaitForFinishCommnadlist() {
	// Fenceの値を更新
	fenceVal_++;
	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceVal_);

	// Fenceの値が指定したSigna値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceVal_) {
		// 指定したSignal値にたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent_);
		// イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}

void DirectXCommand::CreateCommandQueue() {
	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();
	
	// コマンドキューを作成
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateCommandQueue() Failed", "Engine");
		return;
	}
}

void DirectXCommand::CreateCommandAllocator() {
	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();

	// コマンドアロケータを生成する
	commandAllocator_ = nullptr;
	HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateCommandAllocator() Failed", "Engine");
		return;
	}
}

void DirectXCommand::CreateGraphicsCommandList() {
	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();
	
	// コマンドリストを作成する
	commandList_ = nullptr;
	HRESULT hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateCommandList() Failed", "Engine");
		return;
	}
}

void DirectXCommand::CrateFence() {
	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();
	
	// 初期値0でFenceを作る
	fence_ = nullptr;
	fenceVal_ = 0llu;
	HRESULT hr = device->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateFence() Failed", "Engine");
		return;
	}

	// FenceのSignalを持つためのイベントを作成する
	fenceEvent_ = nullptr;
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
	if (!(fenceEvent_ != nullptr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateEvent() Failed", "Engine");
		return;
	}
}