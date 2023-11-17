#pragma once
#include <memory>
#include <vector>
#include <list>
#include <deque>
#include "Engine/StructuredBuffer/StructuredBuffer.h"
#include "TextureManager/Texture/Texture.h"
#include "Engine/RenderTarget/RenderTarget.h"

/// <summary>
/// ディスクリプタヒープ管理クラス
/// </summary>
class CbvSrvUavHeap {
private:
	CbvSrvUavHeap() = delete;
	CbvSrvUavHeap(const CbvSrvUavHeap& right) = delete;
	CbvSrvUavHeap(CbvSrvUavHeap&& right) noexcept = delete;
	CbvSrvUavHeap(UINT numDescriptor);
	~CbvSrvUavHeap();
	CbvSrvUavHeap& operator=(const CbvSrvUavHeap& right) = delete;
	CbvSrvUavHeap& operator=(CbvSrvUavHeap&& right) noexcept = delete;

public:
	static void Initialize(UINT numDescriptor);

	static void Finalize();

	static CbvSrvUavHeap* GetInstance();

private:
	static CbvSrvUavHeap* instance_;

public:
	void SetHeap();

	void Use(D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT rootParmIndex);
	void Use(uint32_t handleIndex, UINT rootParmIndex);

	/// <summary>
	/// CBVを作成
	/// </summary>
	/// <typeparam name="T">ポインタ型に制限</typeparam>
	/// <param name="conBuf">ConstBuffer</param>
	/// <returns>作成した場所のハンドル</returns>
	template<IsNotPtrCB T>
	uint32_t CreateConstBufferView(ConstBuffer<T>& conBuf) {
		assert(currentHandleIndex_ < heapSize_);
		if (currentHandleIndex_ >= heapSize_) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateConstBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		if (bookingHandle_.empty()) {
			useHandle_.push_back(currentHandleIndex_);
			conBuf.CrerateView(heapHandles_[currentHandleIndex_].first, heapHandles_[currentHandleIndex_].second, currentHandleIndex_);
			currentHandleIndex_++;
			return currentHandleIndex_ - 1u;
		}
		else {
			uint32_t nowCreateViewHandle = bookingHandle_.front();
			useHandle_.push_back(nowCreateViewHandle);
			conBuf.CrerateView(heapHandles_[nowCreateViewHandle].first, heapHandles_[nowCreateViewHandle].second, nowCreateViewHandle);
			bookingHandle_.pop_front();
			return nowCreateViewHandle;
		}

	}

	/// <summary>
	/// CBVを作成
	/// </summary>
	/// <typeparam name="T">ポインタ型に制限</typeparam>
	/// <param name="conBuf">ConstBuffer</param>
	/// <param name="heapIndex">作成する場所のハンドル</param>
	template<IsNotPtrCB T>
	void CreateConstBufferView(ConstBuffer<T>& conBuf, UINT heapIndex) {
		assert(heapIndex < heapSize_);
		if (heapIndex >= heapSize_) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateConstBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		conBuf.CrerateView(heapHandles_[heapIndex].first, heapHandles_[heapIndex].second, heapIndex);
	}

	/// <summary>
	/// SRVを作成
	/// </summary>
	/// <typeparam name="T">ポインタ型に制限</typeparam>
	/// <param name="strcBuf">StructuredBuffer</param>
	/// <returns>作成した場所のハンドル</returns>
	template<IsNotPtrSB T>
	uint32_t CreateStructuredBufferView(StructuredBuffer<T>& strcBuf) {
		assert(currentHandleIndex_ < heapSize_);
		if (currentHandleIndex_ >= heapSize_) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateStructuredBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}
		if (bookingHandle_.empty()) {
			useHandle_.push_back(currentHandleIndex_);
			strcBuf.CrerateView(heapHandles_[currentHandleIndex_].first, heapHandles_[currentHandleIndex_].second, currentHandleIndex_);
			currentHandleIndex_++;
			return currentHandleIndex_ - 1u;
		}
		else {
			uint32_t nowCreateViewHandle = bookingHandle_.front();
			useHandle_.push_back(nowCreateViewHandle);
			strcBuf.CrerateView(heapHandles_[nowCreateViewHandle].first, heapHandles_[nowCreateViewHandle].second, nowCreateViewHandle);
			bookingHandle_.pop_front();
			return nowCreateViewHandle;
		}
	}

	/// <summary>
	/// SRVを作成
	/// </summary>
	/// <typeparam name="T">ポインタ型に制限</typeparam>
	/// <param name="strcBuf">StructuredBuffe</param>
	/// <param name="heapIndex">作成する場所のハンドル</param>
	template<IsNotPtrSB T>
	void CreateStructuredBufferView(StructuredBuffer<T>& strcBuf, UINT heapIndex) {
		assert(heapIndex < heapSize_);
		if (heapIndex >= heapSize_) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateStructuredBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}
		
		strcBuf.CrerateView(heapHandles_[heapIndex].first, heapHandles_[heapIndex].second, heapIndex);
	}

	/// <summary>
	/// テクスチャのビューを作成
	/// </summary>
	/// <param name="tex">Texture</param>
	/// <returns>作成した場所のハンドル</returns>
	uint32_t CreateTxtureView(Texture* tex);

	/// <summary>
	/// テクスチャのビューを作成
	/// </summary>
	/// <param name="tex">Texture</param>
	/// <param name="heapIndex">作成する場所のハンドル</param>
	void CreateTxtureView(Texture* tex, uint32_t heapIndex);

	/// <summary>
	/// ペラポリ用のビュー作成
	/// </summary>
	/// <param name="renderTarget">RenderTarget</param>
	/// <returns>作成した場所のハンドル</returns>
	uint32_t CreatePerarenderView(RenderTarget& renderTarget);

	D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHeapHandle(uint32_t heapIndex) {
		return heapHandles_[heapIndex].first;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHeapHandle(uint32_t heapIndex) {
		return heapHandles_[heapIndex].second;
	}
	
	inline UINT GetSize() const {
		return heapSize_;
	}

	inline ID3D12DescriptorHeap* Get() const {
		return heap_.Get();
	}

	inline ID3D12DescriptorHeap* const* GetAddressOf() const {
		return heap_.GetAddressOf();
	}

public:
	uint32_t BookingHeapPos(UINT nextCreateViewNum);
	void ReleaseView(UINT viewHandle);

	/// <summary>
	/// Useハンドルコンテナに追加(既に追加済みなら追加されない)
	/// </summary>
	/// <param name=""></param>
	void UseThisPosition(uint32_t handle);

private:
	void Reset();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;

	UINT heapSize_;
	UINT currentHandleIndex_;

	std::list<uint32_t> releaseHandle_;
	std::list<uint32_t> useHandle_;
	std::deque<uint32_t> bookingHandle_;


	std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>> heapHandles_;
};