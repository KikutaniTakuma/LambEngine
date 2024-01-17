#pragma once
#include "Texture/Texture.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <queue>
#include <thread>

/// <summary>
/// テクスチャの管理
/// </summary>
class TextureManager {
private:
	TextureManager();
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) noexcept = delete;

	TextureManager& operator=(const TextureManager&) = delete;
	TextureManager& operator=(TextureManager&&) noexcept = delete;
public:
	~TextureManager();

public:
	static TextureManager* const GetInstance();

	static void Initialize();

	static void Finalize();

private:
	static TextureManager* instance_;


public:
	Texture* const LoadTexture(const std::string& fileName);

private:
	Texture* const LoadTexture(const std::string& fileName, ID3D12GraphicsCommandList* const commandList_);

public:
	/// <summary>
	/// スレッドを分けてロードする用
	/// </summary>
	/// <param name="fileName"></param>
	/// <param name="texPtr"></param>
	void LoadTexture(const std::string& fileName, Texture** const texPtr);

	void ThreadLoadTexture();

	Texture* const GetWhiteTex();

	void ReleaseIntermediateResource();

	bool IsNowThreadLoading() const {
		return isNowThreadLoading_ && !isCloaseCommandList_;
	}

	inline ID3D12GraphicsCommandList* const GetCommandList() const {
		return commandList_.Get();
	}

	void ResetCommandList();

	void Use(uint32_t texIndex, UINT rootParam);


private:
	class CbvSrvUavHeap* srvHeap_;

	Lamb::LambPtr<ID3D12CommandQueue> commandQueue_;
	Lamb::LambPtr<ID3D12CommandAllocator> commandAllocator_;
	Lamb::LambPtr<ID3D12GraphicsCommandList> commandList_;
	bool isCloaseCommandList_;

	Lamb::LambPtr<ID3D12Fence> fence_;
	uint64_t fenceVal_;
	HANDLE fenceEvent_;

	/// <summary>
	/// Textureのコンテナ(キー値: ファイルネーム  コンテナデータ型: Texture*)
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures_;
	bool thisFrameLoadFlg_;

	std::queue<std::pair<std::string, Texture** const>> threadTextureBuff_;
	std::thread load_;
	bool isThreadFinish_;
	bool isNowThreadLoading_;
};