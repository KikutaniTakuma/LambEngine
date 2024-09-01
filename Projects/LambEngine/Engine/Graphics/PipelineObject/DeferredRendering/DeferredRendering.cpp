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

void DeferredRendering::Draw() {
	Lamb::SafePtr commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();

	pipelinesNoDepth_[Pipeline::Blend::None]->Use();

	commandList->SetGraphicsRootDescriptorTable(0, deferredRenderingData_.GetHandleGPU());
	commandList->SetGraphicsRootDescriptorTable(1, lights_.GetHandleGPU());

	commandList->SetGraphicsRootDescriptorTable(2, colorTextureHandle_);
	commandList->SetGraphicsRootDescriptorTable(3, normalTextureHandle_);
	commandList->SetGraphicsRootDescriptorTable(4, worldPositionTextureHandle_);
	//commandList->SetGraphicsRootDescriptorTable(5, environmentTextureHandle_);
	commandList->DrawInstanced(3, 1, 0, 0);
}

void DeferredRendering::Use(Pipeline::Blend blendType, bool isDepth) {
	if (isDepth) {
		pipelines_[blendType]->Use();
	}
	else {
		pipelinesNoDepth_[blendType]->Use();
	}
	auto* const commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();
	commandList->SetGraphicsRootDescriptorTable(0, deferredRenderingData_.GetHandleGPU());
	commandList->SetGraphicsRootDescriptorTable(1, lights_.GetHandleGPU());

	commandList->SetGraphicsRootDescriptorTable(2, colorTextureHandle_);
	commandList->SetGraphicsRootDescriptorTable(3, normalTextureHandle_);
	commandList->SetGraphicsRootDescriptorTable(4, worldPositionTextureHandle_);
}

void DeferredRendering::Init(
	const std::string& vsShader,
	const std::string& psShader,
	std::initializer_list<DXGI_FORMAT> formtats
) {
	this->LoadShader(
		vsShader,
		psShader
	);

	std::array<D3D12_DESCRIPTOR_RANGE, 1> cbvRange = {};
	cbvRange[0].BaseShaderRegister = 0;
	cbvRange[0].NumDescriptors = 2;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> lightRange = {};
	lightRange[0].BaseShaderRegister = 0;
	lightRange[0].NumDescriptors = 1;
	lightRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	lightRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> diffColorTextureRange = {};
	diffColorTextureRange[0].BaseShaderRegister = 1;
	diffColorTextureRange[0].NumDescriptors = 1;
	diffColorTextureRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	diffColorTextureRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> diffNormalTextureRange = {};
	diffNormalTextureRange[0].BaseShaderRegister = 2;
	diffNormalTextureRange[0].NumDescriptors = 1;
	diffNormalTextureRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	diffNormalTextureRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> diffWorldPositionTextureRange = {};
	diffWorldPositionTextureRange[0].BaseShaderRegister = 3;
	diffWorldPositionTextureRange[0].NumDescriptors = 1;
	diffWorldPositionTextureRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	diffWorldPositionTextureRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> environmentTextureRange = {};
	environmentTextureRange[0].BaseShaderRegister = 4;
	environmentTextureRange[0].NumDescriptors = 1;
	environmentTextureRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	environmentTextureRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	std::array<D3D12_ROOT_PARAMETER, 5> rootParameter = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = cbvRange.data();
	rootParameter[0].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(cbvRange.size());

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = lightRange.data();
	rootParameter[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(lightRange.size());

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = diffColorTextureRange.data();
	rootParameter[2].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(diffColorTextureRange.size());
	
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = diffNormalTextureRange.data();
	rootParameter[3].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(diffNormalTextureRange.size());

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[4].DescriptorTable.pDescriptorRanges = diffWorldPositionTextureRange.data();
	rootParameter[4].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(diffWorldPositionTextureRange.size());

	/*rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[5].DescriptorTable.pDescriptorRanges = environmentTextureRange.data();
	rootParameter[5].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(environmentTextureRange.size());*/


	RootSignature::Desc desc;
	desc.rootParameter = rootParameter.data();
	desc.rootParameterSize = rootParameter.size();
	desc.samplerDeacs.push_back(
		CreateLinearSampler()
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

	pipelineManager->StateReset();


	CbvSrvUavHeap* const srvHeap = CbvSrvUavHeap::GetInstance();

	lights_.Create(32);

	srvHeap->BookingHeapPos(3u);
	srvHeap->CreateView(deferredRenderingData_);
	srvHeap->CreateView(atomosphericData_);
	srvHeap->CreateView(lights_);
}

DeferredRendering::~DeferredRendering() {
	auto* const srvHeap = CbvSrvUavHeap::GetInstance();
	srvHeap->ReleaseView(deferredRenderingData_.GetHandleUINT());
	srvHeap->ReleaseView(atomosphericData_.GetHandleUINT());
	srvHeap->ReleaseView(lights_.GetHandleUINT());
}