#pragma once
#include "../BaseDrawer.h"
#include "Engine/Graphics/RenderContextManager/RenderContext/RenderContext.h"

/// <summary>
/// 板ポリ描画
/// </summary>
class MeshShaderTest : public BaseDrawer {
public:
	static constexpr uint32_t kMaxDrawCount = 1u;

private:
	static const MeshLoadFileNames kFileNames_;

public:
	MeshShaderTest();
	MeshShaderTest(const MeshShaderTest&) = default;
	MeshShaderTest(MeshShaderTest&&) = default;
	~MeshShaderTest() = default;

	MeshShaderTest& operator=(const MeshShaderTest&) = default;
	MeshShaderTest& operator=(MeshShaderTest&&) = default;

public:
	void Load();

	void Draw(
		const Mat4x4& worldMatrix, 
		const Mat4x4& camera, 
		BlendType blend
	);

	void AllDraw();
};