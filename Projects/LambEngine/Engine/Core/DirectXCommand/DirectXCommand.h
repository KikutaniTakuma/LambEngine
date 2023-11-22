#pragma once

#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#undef max
#undef min
#include <cstdint>

class DirectXCommand {
private:
	DirectXCommand();
	DirectXCommand(const DirectXCommand&) = delete;
	DirectXCommand(DirectXCommand&&) = delete;
	~DirectXCommand();

	DirectXCommand& operator=(const DirectXCommand&) = delete;
	DirectXCommand& operator=(DirectXCommand&&) = delete;

public:
	static DirectXCommand* const GetInstance();

	static void Initialize();
	static void Finalize();

private:
	static DirectXCommand* instance_;

public:
	/// <summary>
	/// コマンドリストを確定させる
	/// </summary>
	void CloseCommandlist();

	/// <summary>
	/// コマンドリストを実行
	/// </summary>
	void ExecuteCommandLists();

	/// <summary>
	/// コマンドリストのリセット(また使えるようにする)
	/// </summary>
	void ResetCommandlist();

	/// <summary>
	/// コマンドリストの実行を待つ
	/// </summary>
	void WaitForFinishCommnadlist();

private:
	void CreateCommandQueue();

	void CreateCommandAllocator();

	void CreateGraphicsCommandList();

	void CrateFence();

public:
	ID3D12GraphicsCommandList* const GetCommandList() const {
		return commandList_.Get();
	}

	ID3D12CommandQueue* const GetCommandQueue() const {
		return commandQueue_.Get();
	}

	inline bool GetIsCloseCommandList() const {
		return isCommandListClose_;
	}

private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	bool isCommandListClose_;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	uint64_t fenceVal_;
	HANDLE fenceEvent_;
};