#pragma once

#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"
#include "Math/Vector2.h"
#include "Math/MathCommon.h"
#include "Engine/Graphics/MeshManager/Mesh/Mesh.h"
#include "Engine/Graphics/GraphicsStructs.h"

class DeferredRendering final : public PeraPipeline {
public:
	struct DeferredRenderingData {
		Vector3 eyePos;
		uint32_t rightNum = 0;
		Directionight directionLight;
		uint32_t isDirectionLight_;
	};
public:
	DeferredRendering() = default;
	DeferredRendering(const DeferredRendering&) = delete;
	DeferredRendering(DeferredRendering&&) = delete;
	~DeferredRendering();

	DeferredRendering& operator=(const DeferredRendering&) = delete;
	DeferredRendering& operator=(DeferredRendering&&) = delete;

public:
	void Init(
		const std::string& vsShader = "./Resources/Shaders/PostShader/Post.VS.hlsl",
		const std::string& psShader = "./Resources/Shaders/PostShader/PostGaussian.PS.hlsl",
		const std::string& gsFileName = {},
		const std::string& hsFileName = {},
		const std::string& dsFileName = {}
	) override;

public:
	void Use(Pipeline::Blend blendType, bool isDepth) override;

	void Update()override;

	void SetDeferredRenderingData(const DeferredRenderingData& deferredRenderingData) {
		*deferredRenderingData_ = deferredRenderingData;
	}

	void SetLight(const PointLight& light, size_t index);

	void Debug(const std::string& guiName);

private:
	ConstantBuffer<DeferredRenderingData> deferredRenderingData_;
	StructuredBuffer<PointLight> lights_;
};
