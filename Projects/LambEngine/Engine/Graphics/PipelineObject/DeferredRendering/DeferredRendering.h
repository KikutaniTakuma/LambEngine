#pragma once

#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"
#include "Math/Vector2.h"
#include "Math/MathCommon.h"
#include "Engine/Graphics/MeshManager/Mesh/Mesh.h"
#include "Engine/Graphics/GraphicsStructs.h"
#include "Drawers/AirSkyBox/AirSkyBox.h"

class DeferredRendering final : public PipelineObject {
public:
	struct DeferredRenderingData {
		Vector3 eyePos;
		uint32_t rightNum = 0;
		DirectionLight directionLight;
		uint32_t isDirectionLight;
		float32_t environmentCoefficient = 1.0f;
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
		const std::string& vsShader = "./Resources/Shaders/DeferredRendering/DeferredRendering.VS.hlsl",
		const std::string& psShader = "./Resources/Shaders/DeferredRendering/DeferredRendering.PS.hlsl",
		std::initializer_list<DXGI_FORMAT> formtats = { DXGI_FORMAT_R32G32B32A32_FLOAT }
	) override;

public:
	void Use(Pipeline::Blend blendType, bool isDepth) override;

	void Draw();

	void SetDeferredRenderingData(const DeferredRenderingData& deferredRenderingData) {
		*deferredRenderingData_ = deferredRenderingData;
	}
	void SetAtmosphericParams(const AirSkyBox::AtmosphericParams& atomosphericData) {
		*atomosphericData_ = atomosphericData;
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
	/*void SetEnvironmentHandle(D3D12_GPU_DESCRIPTOR_HANDLE environmentTextureHandle) {
		environmentTextureHandle_ = environmentTextureHandle;
	}*/

private:
	ConstantBuffer<DeferredRenderingData> deferredRenderingData_;
	ConstantBuffer<AirSkyBox::AtmosphericParams> atomosphericData_;
	StructuredBuffer<PointLight> lights_;
	D3D12_GPU_DESCRIPTOR_HANDLE colorTextureHandle_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE normalTextureHandle_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE worldPositionTextureHandle_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE environmentTextureHandle_ = {};
};
