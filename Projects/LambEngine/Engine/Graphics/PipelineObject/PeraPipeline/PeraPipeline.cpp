#include "PeraPipeline.h"
#include <cassert>
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Utils/Random/Random.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"

#include "Utils/EngineInfo/EngineInfo.h"

void PeraPipeline::Update() {
	*colorBuf_ = color;

	*wvpMat_ = wvp;
}

void PeraPipeline::Use(Pipeline::Blend blendType) {
	pipelines_[blendType]->Use();

	auto* const commandList = DirectXCommand::GetInstance()->GetCommandList();

	render_->UseThisRenderTargetShaderResource();
	commandList->SetGraphicsRootDescriptorTable(1, wvpMat_.GetViewHandle());
}

void PeraPipeline::Init(
	const std::string& vsShader,
	const std::string& psShader,
	const std::string& gsFileName,
	const std::string& hsFileName,
	const std::string& dsFileName
) {
	if (width_ == 0u) {
		width_ = static_cast<uint32_t>(Lamb::ClientSize().x);
	}
	if (height_ == 0u) {
		height_ = static_cast<uint32_t>(Lamb::ClientSize().y);
	}

	render_.reset(new RenderTarget{ width_, height_ });

	this->LoadShader(
		vsShader,
		psShader,
		gsFileName,
		hsFileName,
		dsFileName
	);

	std::array<D3D12_DESCRIPTOR_RANGE, 1> renderRange = {};
	renderRange[0].BaseShaderRegister = 0;
	renderRange[0].NumDescriptors = 1;
	renderRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	renderRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> cbvRange = {};
	cbvRange[0].BaseShaderRegister = 0;
	cbvRange[0].NumDescriptors = 2;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	std::array<D3D12_ROOT_PARAMETER, 2> roootParamater = {};
	roootParamater[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	roootParamater[0].DescriptorTable.pDescriptorRanges = renderRange.data();
	roootParamater[0].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(renderRange.size());

	roootParamater[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	roootParamater[1].DescriptorTable.pDescriptorRanges = cbvRange.data();
	roootParamater[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(cbvRange.size());

	PipelineManager::CreateRootSgnature(roootParamater.data(), roootParamater.size(), true);

	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	PipelineManager::SetShader(shader_);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid);

	PipelineManager::IsDepth(false);

	for (int32_t i = Pipeline::Blend::None; i < Pipeline::Blend::BlendTypeNum; i++) {
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		pipelines_[Pipeline::Blend(i)] = PipelineManager::Create();
	}

	PipelineManager::StateReset();


	auto* const srvHeap = CbvSrvUavHeap::GetInstance();
	srvHeap->BookingHeapPos(3u);
	srvHeap->CreatePerarenderView(*render_);
	srvHeap->CreateConstBufferView(wvpMat_);
	srvHeap->CreateConstBufferView(colorBuf_);
}

PeraPipeline::~PeraPipeline() {
	if (render_) {
		auto* const srvHeap = CbvSrvUavHeap::GetInstance();
		srvHeap->ReleaseView(render_->GetViewHandleUINT());
		srvHeap->ReleaseView(wvpMat_.GetViewHandleUINT());
		srvHeap->ReleaseView(colorBuf_.GetViewHandleUINT());
	}
}