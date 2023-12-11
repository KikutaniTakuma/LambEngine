#pragma once
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include "Math/Vector4.h"
#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"

/// <summary>
/// 線の描画
/// </summary>
class Line {
private:
	struct VertexData {
		Vector4 pos_;
		Vector4 color_;
	};
private:
	static constexpr uint16_t kVertexNum = 2u;
	static constexpr uint16_t kDrawMaxNumber_ = 4096u;
	static uint32_t indexCount_;

public:
	static void Initialize();

	static void Finalize();

	static void ResetDrawCount();

private:
	static Shader shader_;

	static class Pipeline* pipline_;

	static Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	// 頂点バッファビュー
	static D3D12_VERTEX_BUFFER_VIEW vertexView_;

public:
	Line();
	Line(const Line& right);
	Line(Line&& right) noexcept;
	~Line();

	Line& operator=(const Line& right);
	Line& operator=(Line&& right)noexcept;

public:
	void Draw(const Mat4x4& viewProjection, uint32_t color);

public:
	Vector3 start;
	Vector3 end;

private:
	// 頂点バッファマップ
	VertexData* vertexMap_;

	class CbvSrvUavHeap* heap_;

	ConstBuffer<Mat4x4> wvpMat_;
};