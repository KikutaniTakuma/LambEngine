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

private:
	struct ShaderData {
		Vector2 randomVec;
		Vector3 normal;
		float pad = 0.0f;
		Vector3 tangent;
		uint32_t textureID = 0u;
	};

	using WaterRenderContext = RenderContext<ShaderData, kMaxDrawCount>;

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
		uint32_t color,
		BlendType blend
	);
};