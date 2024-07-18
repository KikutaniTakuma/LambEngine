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
	commandList->SetGraphicsRootDescriptorTable(2, lights_.GetHandleGPU());

	commandList->SetGraphicsRootDescriptorTable(3, colorTextureHandle_);
	commandList->SetGraphicsRootDescriptorTable(4, normalTextureHandle_);
	commandList->SetGraphicsRootDescriptorTable(5, worldPositionTextureHandle_);
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

	render_ = std::make_unique<RenderTarget>(width_, height_);

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
	std::array<D3D12_DESCRIPTOR_RANGE, 1> lightRange = {};
	lightRange[0].BaseShaderRegister = 1;
	lightRange[0].NumDescriptors = 1;
	lightRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	lightRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> diffColorTextureRange = {};
	diffColorTextureRange[0].BaseShaderRegister = 2;
	diffColorTextureRange[0].NumDescriptors = 1;
	diffColorTextureRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	diffColorTextureRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> diffNormalTextureRange = {};
	diffNormalTextureRange[0].BaseShaderRegister = 3;
	diffNormalTextureRange[0].NumDescriptors = 1;
	diffNormalTextureRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	diffNormalTextureRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> diffWorldPositionTextureRange = {};
	diffWorldPositionTextureRange[0].BaseShaderRegister = 4;
	diffWorldPositionTextureRange[0].NumDescriptors = 1;
	diffWorldPositionTextureRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	diffWorldPositionTextureRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	std::array<D3D12_ROOT_PARAMETER, 6> rootParameter = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = renderRange.data();
	rootParameter[0].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(renderRange.size());

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = cbvRange.data();
	rootParameter[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(cbvRange.size());

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = lightRange.data();
	rootParameter[2].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(lightRange.size());

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = diffColorTextureRange.data();
	rootParameter[3].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(diffColorTextureRange.size());
	
	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[4].DescriptorTable.pDescriptorRanges = diffNormalTextureRange.data();
	rootParameter[4].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(diffNormalTextureRange.size());

	rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[5].DescriptorTable.pDescriptorRanges = diffWorldPositionTextureRange.data();
	rootParameter[5].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(diffWorldPositionTextureRange.size());


	RootSignature::Desc desc;
	desc.rootParameter = rootParameter.data();
	desc.rootParameterSize = rootParameter.size();
	desc.samplerDeacs.push_back(
		CreateBorderLessSampler()
	);
	desc.samplerDeacs.push_back(
		CreatePointSampler(1)
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
	pipelineDesc.numRenderTarget = 1;
	pipelineDesc.rtvFormtat[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;


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

	pipelineManager->StateReset();


	CbvSrvUavHeap* const srvHeap = CbvSrvUavHeap::GetInstance();

	lights_.Create(32);

	srvHeap->BookingHeapPos(4u);
	srvHeap->CreateView(*render_);
	srvHeap->CreateView(colorBuf_);
	srvHeap->CreateView(deferredRenderingData_);
	srvHeap->CreateView(lights_);
}

DeferredRendering::~DeferredRendering() {
	if (render_) {
		auto* const srvHeap = CbvSrvUavHeap::GetInstance();
		srvHeap->ReleaseView(render_->GetHandleUINT());
		srvHeap->ReleaseView(colorBuf_.GetHandleUINT());
		srvHeap->ReleaseView(deferredRenderingData_.GetHandleUINT());
		srvHeap->ReleaseView(lights_.GetHandleUINT());
	}

	render_.reset();
}