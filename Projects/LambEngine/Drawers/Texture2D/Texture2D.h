#pragma once
#include "../BaseDrawer.h"
#include "Engine/Graphics/RenderContextManager/RenderContext/RenderContext.h"

/// <summary>
/// 板ポリ描画
/// </summary>
class Texture2D : public BaseDrawer {
public:
	struct Data {
		Mat4x4 worldMatrix = Mat4x4::kIdentity;
		Mat4x4 uvTransform = Mat4x4::kIdentity;
		Mat4x4 camera = Mat4x4::kIdentity;
		uint32_t textureID = 0;
		uint32_t color = 0xffffffff;
		BlendType blend = BlendType::kNone;
	};
	struct Instance {
		QuaternionTransform transform;
		uint32_t textureID = 0;
		uint32_t color = 0xffffffff;
	};

public:
	static constexpr uint32_t kMaxDrawCount = 1024u;

private:
	struct ShaderData{
		Mat4x4 uvTransform;
		Vector3 pad; // <- huh?
		uint32_t textureID = 0u;
	};

	using Texture2DRenderContext = RenderContext<ShaderData, kMaxDrawCount>;

	static const LoadFileNames kFileNames_;

public:
	Texture2D();
	Texture2D(const Texture2D&) = default;
	Texture2D(Texture2D&&) = default;
	~Texture2D() = default;

	Texture2D& operator=(const Texture2D&) = default;
	Texture2D& operator=(Texture2D&&) = default;

public:
	void Load();

	void Draw(
		const Mat4x4& worldMatrix, 
		const Mat4x4& uvTransform, 
		const Mat4x4& camera, 
		uint32_t textureID, 
		uint32_t color, 
		BlendType blend
	);
	void Draw(const Texture2D::Data& data);

	void AllDraw();
};