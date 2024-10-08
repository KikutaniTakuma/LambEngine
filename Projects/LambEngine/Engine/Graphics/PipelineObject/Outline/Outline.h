#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"
#include "Math/Vector2.h"
#include "Math/MathCommon.h"
#include "Engine/Graphics/GraphicsStructs.h"

#include "Utils/EngineInfo.h"

class Outline final : public PeraPipeline {
public:
	struct OutlineData{
		float32_t weight = 1.0f;
		float32_t4x4 projectionInverse;
	};

public:
	Outline() = default;
	Outline(const Outline&) = delete;
	Outline(Outline&&) = delete;
	~Outline();

	Outline& operator=(const Outline&) = delete;
	Outline& operator=(Outline&&) = delete;

public:
	void Init(
		const std::string& vsShader = "./Shaders/PostShader/Post.VS.hlsl",
		const std::string& psShader = "./Shaders/PostShader/PostOutline.PS.hlsl",
		std::initializer_list<DXGI_FORMAT> formtats = { DXGI_FORMAT_R8G8B8A8_UNORM_SRGB }
	) override;

public:
	void Use(Pipeline::Blend blendType, bool isDepth = false) override;

	void Update()override;

	void Debug(const std::string& guiName);

	void SetWeight(float32_t weight);
	void SetProjectionInverse(const float32_t4x4& projectionInverse);

	void ChangeDepthBufferState();

private:
	std::array<std::unique_ptr<ConstantBuffer<OutlineData>>, DirectXSwapChain::kBackBufferNumber> outlineData_;
	float32_t weight_ = 0.0f;
};
