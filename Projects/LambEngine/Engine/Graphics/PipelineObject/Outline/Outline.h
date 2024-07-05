#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"
#include "Math/Vector2.h"
#include "Math/MathCommon.h"
#include "Engine/Graphics/MeshManager/Mesh/Mesh.h"
#include "Engine/Graphics/GraphicsStructs.h"

class Outline final : public PeraPipeline {
public:
	Outline() = default;
	Outline(const Outline&) = delete;
	Outline(Outline&&) = delete;
	~Outline();

	Outline& operator=(const Outline&) = delete;
	Outline& operator=(Outline&&) = delete;

public:
	void Init(
		const std::string& vsShader = "./Resources/Shaders/PostShader/Post.VS.hlsl",
		const std::string& psShader = "./Resources/Shaders/PostShader/PostGaussian.PS.hlsl",
		const std::string& gsFileName = {},
		const std::string& hsFileName = {},
		const std::string& dsFileName = {}
	) override;

public:
	void Use(Pipeline::Blend blendType, bool isDepth = false) override;

	void Update()override;

	void Debug(const std::string& guiName);

	void SetWeight(float32_t weight) {
		*weight_ = weight;
	}

	void ChangeDepthBufferState();

private:
	ConstantBuffer<float32_t> weight_;
};
