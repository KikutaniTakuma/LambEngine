#pragma once
#include "../BaseDrawer.h"
#include "Engine/Graphics/RenderContextManager/RenderContext/RenderContext.h"

/// <summary>
/// 板ポリ描画
/// </summary>
class Texture2D : public BaseDrawer {
public:
	static constexpr uint32_t kMaxDrawCount = 1024u;

private:
	struct ShaderData{
		Mat4x4 uvTransform;
		Vector3 pad;
		uint32_t textureID = 0u;
	};

	using Texture2DRenderContext = RenderContext<ShaderData, kMaxDrawCount>;

	static const LoadFileNames kFileNames_;

public:
	Texture2D();
	Texture2D(const std::string& fileName);
	Texture2D(const Texture2D&) = default;
	Texture2D(Texture2D&&) = default;
	~Texture2D() = default;

	Texture2D& operator=(const Texture2D&) = default;
	Texture2D& operator=(Texture2D&&) = default;

public:
	void Load(const std::string& fileName) override;

	void Draw(const Camera* camera, BlendType blend) override;

	void Debug(const std::string& guiName) override;

public:
	Vector3 uvScale;
	Vector3 uvRotate;
	Vector3 uvTranslation;
private:
	uint32_t textureID_;
};