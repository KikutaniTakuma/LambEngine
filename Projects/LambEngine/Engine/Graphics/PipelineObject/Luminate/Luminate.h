#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"
#include "Math/Vector2.h"
#include "Math/MathCommon.h"
#include "Engine/Graphics/MeshManager/Mesh/Mesh.h"
#include "Engine/Graphics/GraphicsStructs.h"

class Luminate final : public PeraPipeline {
public:
	Luminate() = default;
	Luminate(const Luminate&) = delete;
	Luminate(Luminate&&) = delete;
	~Luminate();

	Luminate& operator=(const Luminate&) = delete;
	Luminate& operator=(Luminate&&) = delete;

public:
	void Init(
		const std::string& vsShader = "./Resources/Shaders/PostShader/Post.VS.hlsl",
		const std::string& psShader = "./Resources/Shaders/PostShader/PostLuminate.PS.hlsl",
		std::initializer_list<DXGI_FORMAT> formtats = { DXGI_FORMAT_R32G32B32A32_FLOAT }
	) override;

public:
	void Use(Pipeline::Blend blendType, bool isDepth) override;

	void Update()override;

	void SetLuminanceThreshold(float32_t luminanceThreshold) {
		*luminanceThreshold_ = luminanceThreshold;
	}

	void SetRtvFormt(DXGI_FORMAT format);

	void Debug();

private:
	ConstantBuffer<float32_t> luminanceThreshold_;
	DXGI_FORMAT format_ = DXGI_FORMAT_R32G32B32A32_FLOAT;
};
