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
		DirectionLight directionLight;
		uint32_t isDirectionLight;
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
		const std::string& psShader = "./Resources/Shaders/PostShader/PostDeferred.PS.hlsl",
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

	void SetColorHandle(D3D12_GPU_DESCRIPTOR_HANDLE colorTextureHandle) {
		colorTextureHandle_ = colorTextureHandle;
	}
	void SetNormalHandle(D3D12_GPU_DESCRIPTOR_HANDLE normalTextureHandle) {
		normalTextureHandle_ = normalTextureHandle;
	}
	void SetWoprldPositionHandle(D3D12_GPU_DESCRIPTOR_HANDLE worldPositionTextureHandle) {
		worldPositionTextureHandle_ = worldPositionTextureHandle;
	}

private:
	ConstantBuffer<DeferredRenderingData> deferredRenderingData_;
	StructuredBuffer<PointLight> lights_;
	D3D12_GPU_DESCRIPTOR_HANDLE colorTextureHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE normalTextureHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE worldPositionTextureHandle_;
};