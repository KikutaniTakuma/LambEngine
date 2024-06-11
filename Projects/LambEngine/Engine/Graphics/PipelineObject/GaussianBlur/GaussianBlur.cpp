#include "GaussianBlur.h"
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


void GaussianBlur::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	if (ImGui::TreeNode("GaussianBlurState")) {
		ImGui::DragInt("kernel", &gaussianBlurState_->kernelSize, 0.1f, 0, 64);
		ImGui::DragFloat2("dir", gaussianBlurState_->dir.data(), 1.0f, 0.0f, 1.0f);
		ImGui::DragFloat("dir", &gaussianBlurState_->sigma, 0.01f, 0.0f, 10.0f);
		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG

}

void GaussianBlur::Update() {
	*colorBuf_ = color;

	wvpMat_->worldMat = worldMat;
	wvpMat_->viewProjection = viewProjection;
}

void GaussianBlur::Use(Pipeline::Blend blendType, bool isDepth) {
	if (isDepth) {
		pipelines_[blendType]->Use();
	}
	else {
		pipelinesNoDepth_[blendType]->Use();
	}
	auto* const commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();

	render_->UseThisRenderTargetShaderResource();
	commandList->SetGraphicsRootDescriptorTable(1, wvpMat_.GetHandleGPU());
}

void GaussianBlur::Init(
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
	std::array<D3D12_DESCRIPTOR_RANGE, 1> causticsRange = {};
	causticsRange[0].BaseShaderRegister = 1;
	causticsRange[0].NumDescriptors = 1;
	causticsRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	causticsRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> cbvRange = {};
	cbvRange[0].BaseShaderRegister = 0;
	cbvRange[0].NumDescriptors = 3;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	std::array<D3D12_ROOT_PARAMETER, 3> roootParamater = {};
	roootParamater[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	roootParamater[0].DescriptorTable.pDescriptorRanges = renderRange.data();
	roootParamater[0].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(renderRange.size());

	roootParamater[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	roootParamater[1].DescriptorTable.pDescriptorRanges = causticsRange.data();
	roootParamater[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(causticsRange.size());

	roootParamater[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	roootParamater[2].DescriptorTable.pDescriptorRanges = cbvRange.data();
	roootParamater[2].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(cbvRange.size());

	PipelineManager::CreateRootSgnature(roootParamater.data(), roootParamater.size(), true);

	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	PipelineManager::SetShader(shader_);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid);

	for (int32_t i = Pipeline::Blend::None; i < Pipeline::Blend::BlendTypeNum; i++) {
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		PipelineManager::IsDepth(true);
		pipelines_[Pipeline::Blend(i)] = PipelineManager::Create();

		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		PipelineManager::IsDepth(false);
		pipelinesNoDepth_[Pipeline::Blend(i)] = PipelineManager::Create();
	}

	PipelineManager::StateReset();


	CbvSrvUavHeap* const srvHeap = CbvSrvUavHeap::GetInstance();

	srvHeap->BookingHeapPos(4u);
	srvHeap->CreateView(*render_);
	srvHeap->CreateView(wvpMat_);
	srvHeap->CreateView(colorBuf_);
	srvHeap->CreateView(gaussianBlurState_);
}

GaussianBlur::~GaussianBlur() {
	if (render_) {
		auto* const srvHeap = CbvSrvUavHeap::GetInstance();
		srvHeap->ReleaseView(render_->GetHandleUINT());
		srvHeap->ReleaseView(wvpMat_.GetHandleUINT());
		srvHeap->ReleaseView(colorBuf_.GetHandleUINT());
		srvHeap->ReleaseView(gaussianBlurState_.GetHandleUINT());
	}

	render_.reset();
}