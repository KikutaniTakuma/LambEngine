#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"

class WaterPipeline final : public PeraPipeline {
public:
	WaterPipeline() = default;
	WaterPipeline(const WaterPipeline&) = delete;
	WaterPipeline(WaterPipeline&&) = delete;
	~WaterPipeline();

	WaterPipeline& operator=(const WaterPipeline&) = delete;
	WaterPipeline& operator=(WaterPipeline&&) = delete;

public:
	void Init(
		const std::string& vsShader = "./Resources/Shaders/PostShader/Post.VS.hlsl",
		const std::string& psShader = "./Resources/Shaders/PostShader/PostPerlin.PS.hlsl",
		const std::string& gsFileName = {},
		const std::string& hsFileName = {},
		const std::string& dsFileName = {}
	) override;

public:
	void Update()override;

private:
	ConstBuffer<Vector2> randomVec_;
};
