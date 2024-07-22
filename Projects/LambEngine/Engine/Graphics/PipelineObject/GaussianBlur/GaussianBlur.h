#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"
#include "Math/Vector2.h"
#include "Math/MathCommon.h"
#include "Engine/Graphics/MeshManager/Mesh/Mesh.h"
#include "Engine/Graphics/GraphicsStructs.h"

class GaussianBlur final : public PeraPipeline {
public:
	struct GaussianBlurState {
		Vector2 dir;
		float32_t sigma;
		int32_t kernelSize;
	};
public:
	GaussianBlur() = default;
	GaussianBlur(const GaussianBlur&) = delete;
	GaussianBlur(GaussianBlur&&) = delete;
	~GaussianBlur();

	GaussianBlur& operator=(const GaussianBlur&) = delete;
	GaussianBlur& operator=(GaussianBlur&&) = delete;

public:
	void Init(
		const std::string& vsShader = "./Resources/Shaders/PostShader/Post.VS.hlsl",
		const std::string& psShader = "./Resources/Shaders/PostShader/PostGaussian.PS.hlsl",
		std::initializer_list<DXGI_FORMAT> formtats = { DXGI_FORMAT_R32G32B32A32_FLOAT }
	) override;

public:
	void Use(Pipeline::Blend blendType, bool isDepth) override;

	void Update()override;

	void SetGaussianState(const GaussianBlurState& gaussianBlurState) {
		*gaussianBlurState_ = gaussianBlurState;
	}

	void Debug(const std::string& guiName);

private:
	ConstantBuffer<GaussianBlurState> gaussianBlurState_;
};
