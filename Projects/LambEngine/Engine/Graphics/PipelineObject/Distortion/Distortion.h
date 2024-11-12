#pragma once
#include "../PipelineObject.h"
#include "../PeraPipeline/PeraPipeline.h"
#include "Math/Vector2.h"
#include "Math/MathCommon.h"
#include "Engine/Graphics/GraphicsStructs.h"

#include "Engine/Graphics/Tonemap/Tonemap.h"

class Distortion final : public PeraPipeline {
public:
	Distortion() = default;
	Distortion(const Distortion&) = delete;
	Distortion(Distortion&&) = delete;
	~Distortion();

	Distortion& operator=(const Distortion&) = delete;
	Distortion& operator=(Distortion&&) = delete;

public:
	void Init(
		const std::string& vsShader = "./Shaders/PostShader/Post.VS.hlsl",
		const std::string& psShader = "./Shaders/PostShader/PostDistortion.PS.hlsl",
		std::initializer_list<DXGI_FORMAT> formtats = { 
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			DXGI_FORMAT_R32G32B32A32_FLOAT 
		}
	) override;

public:
	void Use(Pipeline::Blend blendType, bool isDepth) override;

	void SetRtvFormt(DXGI_FORMAT format);

	void SetDistortionTexHandle(D3D12_GPU_DESCRIPTOR_HANDLE distortionTexHandle) {
		distortionTexHandle_ = distortionTexHandle;
	}

	void SetDepthTexHandle(D3D12_GPU_DESCRIPTOR_HANDLE depthTexHandle) {
		depthTexHandle_ = depthTexHandle;
	}

	void SetCausticsTexHandle(D3D12_GPU_DESCRIPTOR_HANDLE causticsTexHandle) {
		causticsTexHandle_ = causticsTexHandle;
	}

	void SetTonemapParams(const TonemapParams& tonemapParams);

private:
	std::array<std::unique_ptr<ConstantBuffer<float32_t4x4>>, DirectXSwapChain::kBackBufferNumber> scrollUVBuf_;
	std::array<std::unique_ptr<ConstantBuffer<float32_t>>, DirectXSwapChain::kBackBufferNumber> depthFloatBuf_;
	std::array<std::unique_ptr<ConstantBuffer<TonemapParams>>, DirectXSwapChain::kBackBufferNumber> tonemapParamasBuf_;
	float32_t4x4 scrollUV_;
	float32_t depthFloat_ = 0.423f;
	TonemapParams tonemapParamas_;

	Vector3 translate_;
	Vector3 uvScrollSpeed_ = Vector3::kXIdentity * 0.05f + Vector3::kYIdentity * 0.05f;

	DXGI_FORMAT format_ = DXGI_FORMAT_R32G32B32A32_FLOAT;
	D3D12_GPU_DESCRIPTOR_HANDLE distortionTexHandle_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE depthTexHandle_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE causticsTexHandle_ = {};
};
