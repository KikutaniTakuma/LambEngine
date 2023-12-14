//#pragma once
//#include "../PipelineObject.h"
//#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"
//#include "Math/Vector2.h"
//#include "Math/Mat4x4.h"
//
//class WaterPipeline final : public PipelineObject {
//public:
//	WaterPipeline() = default;
//	WaterPipeline(const WaterPipeline&) = delete;
//	WaterPipeline(WaterPipeline&&) = delete;
//	~WaterPipeline() = default;
//
//	WaterPipeline& operator=(const WaterPipeline&) = delete;
//	WaterPipeline& operator=(WaterPipeline&&) = delete;
//
//public:
//	void Use() override;
//
//	void Init(
//		const std::string& vsShader = "./Resources/Shaders/PostShader/Post.VS.hlsl",
//		const std::string& psShader = "./Resources/Shaders/PostShader/PostPerlin.PS..hlsl"
//	) override;
//
//public:
//	Mat4x4 wvp;
//	Vector4 color;
//
//private:
//	ConstBuffer<Mat4x4> wvpMat_;
//	ConstBuffer<Vector4> colorBuf_;
//	ConstBuffer<Vector2> randomVec_;
//};