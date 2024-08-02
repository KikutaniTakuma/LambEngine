#include "Luminate.h"
#include <cassert>
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Utils/Random.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

#include "Utils/EngineInfo.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void Luminate::SetRtvFormt(DXGI_FORMAT format) {
	format_ = format;
}

void Luminate::Debug() {
#ifdef _DEBUG
	ImGui::DragFloat("輝度閾値", &(*luminanceThreshold_), 0.001f, 0.0f, 1.0f);
#endif // _DEBUG
}

void Luminate::Update() {
	*colorBuf_ = color;
}

void Luminate::Use(Pipeline::Blend blendType, bool isDepth) {
	if (isDepth) {
		pipelines_[blendType]->Use();
	}
	else {
		pipelinesNoDepth_[blendType]->Use();
	}
	auto* const commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();

	render_->UseThisRenderTargetShaderResource();
	commandList->SetGraphicsRootDescriptorTable(1, colorBuf_.GetHandleGPU());
}

void Luminate::Init(
	const std::string& vsShader,
	const std::string& psShader,
	std::initializer_list<DXGI_FORMAT> formtats
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
		psShader
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

	std::array<D3D12_ROOT_PARAMETER, 2> rootParameter = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = renderRange.data();
	rootParameter[0].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(renderRange.size());

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = cbvRange.data();
	rootParameter[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(cbvRange.size());


	RootSignature::Desc desc;
	desc.rootParameter = rootParameter.data();
	desc.rootParameterSize = rootParameter.size();
	desc.samplerDeacs.push_back(
		CreateBorderSampler()
	);

	auto pipelineManager = PipelineManager::GetInstance();
	Pipeline::Desc pipelineDesc;
	pipelineDesc.rootSignature = pipelineManager->CreateRootSgnature(desc, true);
	pipelineDesc.vsInputData.clear();
	pipelineDesc.shader = shader_;
	pipelineDesc.isDepth = false;
	pipelineDesc.blend[0] = Pipeline::None;
	pipelineDesc.solidState = Pipeline::SolidState::Solid;
	pipelineDesc.cullMode = Pipeline::CullMode::Back;
	pipelineDesc.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.numRenderTarget = uint32_t(formtats.size());
	for (uint32_t count = 0; const auto & i : formtats) {
		pipelineDesc.rtvFormtat[count] = i;
		count++;
		if (8 <= count) {
			break;
		}
	}


	for (int32_t i = Pipeline::Blend::None; i < Pipeline::Blend::BlendTypeNum; i++) {
		for (auto& blend : pipelineDesc.blend) {
			blend = Pipeline::Blend(i);
		}
		pipelineDesc.isDepth = true;
		pipelineManager->SetDesc(pipelineDesc);
		pipelines_[Pipeline::Blend(i)] = pipelineManager->Create();

		pipelineDesc.isDepth = false;
		pipelineManager->SetDesc(pipelineDesc);
		pipelinesNoDepth_[Pipeline::Blend(i)] = pipelineManager->Create();
	}


	CbvSrvUavHeap* const srvHeap = CbvSrvUavHeap::GetInstance();

	srvHeap->BookingHeapPos(3u);
	srvHeap->CreateView(*render_);
	srvHeap->CreateView(colorBuf_);
	srvHeap->CreateView(luminanceThreshold_);
}

Luminate::~Luminate() {
	if (render_) {
		auto* const srvHeap = CbvSrvUavHeap::GetInstance();
		srvHeap->ReleaseView(render_->GetHandleUINT());
		srvHeap->ReleaseView(colorBuf_.GetHandleUINT());
		srvHeap->ReleaseView(luminanceThreshold_.GetHandleUINT());
	}

	render_.reset();
}