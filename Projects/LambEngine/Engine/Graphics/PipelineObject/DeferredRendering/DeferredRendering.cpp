#include "DeferredRendering.h"
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


void DeferredRendering::SetLight(const PointLight& light, size_t index)
{
	lights_[index] = light;
}

void DeferredRendering::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef _DEBUG
	
#endif // _DEBUG

}

void DeferredRendering::Update() {
	*colorBuf_ = color;
}

void DeferredRendering::Use(Pipeline::Blend blendType, bool isDepth) {
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

void DeferredRendering::Init(
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
	cbvRange[0].NumDescriptors = 3;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> lightRange = {};
	lightRange[0].BaseShaderRegister = 1;
	lightRange[0].NumDescriptors = 1;
	lightRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	lightRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> diffTextureRange = {};
	diffTextureRange[0].BaseShaderRegister = 2;
	diffTextureRange[0].NumDescriptors = 2;
	diffTextureRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	diffTextureRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	std::array<D3D12_ROOT_PARAMETER, 4> rootParameter = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = renderRange.data();
	rootParameter[0].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(renderRange.size());

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = cbvRange.data();
	rootParameter[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(cbvRange.size());

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = lightRange.data();
	rootParameter[2].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(lightRange.size());

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = diffTextureRange.data();
	rootParameter[3].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(diffTextureRange.size());


	RootSignature::Desc desc;
	desc.rootParameter = rootParameter.data();
	desc.rootParameterSize = rootParameter.size();
	desc.samplerDeacs.push_back(
		CreateBorderLessSampler()
	);

	auto pipelineManager = PipelineManager::GetInstance();
	Pipeline::Desc pipelineDesc;
	pipelineDesc.rootSignature = pipelineManager->CreateRootSgnature(desc, true);
	pipelineDesc.vsInputData.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT });
	pipelineDesc.vsInputData.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT });
	pipelineDesc.shader = shader_;
	pipelineDesc.isDepth = false;
	pipelineDesc.blend[0] = Pipeline::None;
	pipelineDesc.solidState = Pipeline::SolidState::Solid;
	pipelineDesc.cullMode = Pipeline::CullMode::Back;
	pipelineDesc.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.numRenderTarget = 1;


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
	srvHeap->CreateView(deferredRenderingData_);
}

DeferredRendering::~DeferredRendering() {
	if (render_) {
		auto* const srvHeap = CbvSrvUavHeap::GetInstance();
		srvHeap->ReleaseView(render_->GetHandleUINT());
		srvHeap->ReleaseView(colorBuf_.GetHandleUINT());
		srvHeap->ReleaseView(deferredRenderingData_.GetHandleUINT());
	}

	render_.reset();
}