#pragma once
#include "../BaseDrawer.h"
#include "Engine/Graphics/RenderContextManager/RenderContext/RenderContext.h"

/// <summary>
/// 板ポリ描画
/// </summary>
class MeshShaderTest : public BaseDrawer {
public:
	static constexpr uint32_t kMaxDrawCount = 1u;

public:
	MeshShaderTest();
	MeshShaderTest(const MeshShaderTest&) = default;
	MeshShaderTest(MeshShaderTest&&) = default;
	~MeshShaderTest() = default;

	MeshShaderTest& operator=(const MeshShaderTest&) = default;
	MeshShaderTest& operator=(MeshShaderTest&&) = default;

public:
	void Load(const std::string& fileName);

	void Draw(
		const Mat4x4& worldMatrix, 
		const Mat4x4& camera,
		uint32_t color,
		bool isLighting,
		BlendType blend
	);

	void AllDraw();
};