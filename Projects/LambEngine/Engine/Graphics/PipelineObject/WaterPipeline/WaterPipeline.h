#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"
#include "Math/Vector3.h"
#include "Engine/Graphics/MeshManager/Mesh/Mesh.h"

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
		const std::string& vsShader = "./Resources/Shaders/PostShader/PostWater.VS.hlsl",
		const std::string& psShader = "./Resources/Shaders/PostShader/PostWater.PS.hlsl",
		const std::string& gsFileName = {},
		const std::string& hsFileName = {},
		const std::string& dsFileName = {}
	) override;

public:
	void Update()override;

	void SetCameraPos(const Vector3& cameraPos) {
		light_->eyePos = cameraPos;
	}

	void SetRotate(const Vector3& rotate) {
		*normalRotate_ = Mat4x4::MakeRotate(rotate);
	}

private:
	ConstBuffer<Vector2> randomVec_;
	ConstBuffer<Mat4x4> normalRotate_;
	ConstBuffer<Mesh::Light> light_;
};
