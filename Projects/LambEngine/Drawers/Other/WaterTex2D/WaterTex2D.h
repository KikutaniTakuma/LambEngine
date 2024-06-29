#pragma once

#pragma once
#include "../../BaseDrawer.h"
#include "Engine/Graphics/RenderContextManager/RenderContext/RenderContext.h"

/// <summary>
/// 板ポリ描画
/// </summary>
class WaterTex2D : public BaseDrawer {
public:
	static constexpr uint32_t kMaxDrawCount = 1024u;

	struct ShaderData {
		Vector2 randomVec;
		Vector3 normal;
		Vector3 tangent;
		uint32_t textureID = 0u;
		float32_t density = 0.0_f32;
		uint32_t edgeDivision = 1;
		uint32_t insideDivision = 1;
	};

	using WaterRenderContext = RenderContext<ShaderData, kMaxDrawCount>;
private:

	static const LoadFileNames kFileNames_;

	static uint32_t kCausticsTextureID_;

public:
	WaterTex2D();
	WaterTex2D(const WaterTex2D&) = default;
	WaterTex2D(WaterTex2D&&) = default;
	~WaterTex2D() = default;

	WaterTex2D& operator=(const WaterTex2D&) = default;
	WaterTex2D& operator=(WaterTex2D&&) = default;

public:
	void Load();

	void Draw(
		const Mat4x4& worldMatrix,
		const Mat4x4& camera,
		Vector2 randomVec,
		float32_t density,
		uint32_t edgeDivision,
		uint32_t insideDivision,
		uint32_t color,
		BlendType blend
	);
	void AllDraw(BlendType blend);
};