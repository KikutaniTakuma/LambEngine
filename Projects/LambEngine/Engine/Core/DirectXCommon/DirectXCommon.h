#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#undef max
#undef min
#include <vector>
#include <cstdint>
#include <array>

/// <summary>
/// DirectX12のデバイス以外を管理(スワップチェイン、コマンドリストなど)
/// </summary>
class DirectXCommon {
private:
	DirectXCommon();
	DirectXCommon(const DirectXCommon&) = delete;
	DirectXCommon(DirectXCommon&&) = delete;
	~DirectXCommon();

public:
	DirectXCommon& operator=(const DirectXCommon&) = delete;
	DirectXCommon& operator=(DirectXCommon&&) = delete;

public:
	static DirectXCommon* GetInstance();

	static void Initialize();
	static void Finalize();

private:
	static DirectXCommon* instance_;

public:
	/// <summary>
	/// ビューポートセット
	/// </summary>
	/// <param name="width">基本的にバックバッファと同じにする</param>
	/// <param name="height">基本的にバックバッファと同じにする</param>
	void SetViewPort(uint32_t width, uint32_t height);

	/// <summary>
	/// バリア
	/// </summary>
	/// <param name="resource">リソースバリアを貼るリソース</param>
	/// <param name="before">今の状態</param>
	/// <param name="after">遷移後の状態</param>
	/// <param name="subResource">サブリソース</param>
	void Barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, UINT subResource = 0u);

	/// <summary>
	/// メインレンダーターゲットにセットする
	/// </summary>
	void SetMainRenderTarget();

	void ClearBackBuffer();

	/// <summary>
	/// バックバッファの状態を変更する
	/// </summary>
	void ChangeBackBufferState();

	/// <summary>
	/// スワップチェインに画面の交換を行う
	/// </summary>
	void SwapChainPresent();

public:
	IDXGISwapChain4* GetSwapChain() const {
		return swapChain_.Get();
	}

	inline D3D12_RESOURCE_DESC GetSwapchainBufferDesc() const {
		return swapChainResource_[0]->GetDesc();
	}

public:
	static constexpr uint32_t kBackBufferNumber_ = 2u;

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kBackBufferNumber_> swapChainResource_;
	bool isRenderState_;
};