#include "RenderContextManager.h"
#include "Utils/SafeDelete/SafeDelete.h"

#include "../Shader/ShaderManager/ShaderManager.h"

#include "../../Core/DescriptorHeap/CbvSrvUavHeap.h"

#include "../PipelineManager/PipelineManager.h"

#include "../MeshLoader/MeshLoader.h"

RenderContextManager::~RenderContextManager()
{
	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	for (auto& i : renderData_) {
		descriptorHeap->ReleaseView(i.second->light.GetHandleUINT());
		descriptorHeap->ReleaseView(i.second->shaderStruct.GetHandleUINT());
		descriptorHeap->ReleaseView(i.second->wvpMatrix.GetHandleUINT());
	}
}

RenderContextManager* const RenderContextManager::GetInstance() {
	return instance_;
}

void RenderContextManager::Initialize() {
	if (!instance_) {
		instance_ = new RenderContextManager();
	}
}

void RenderContextManager::Finalize() {
	if (instance_) {
		Lamb::SafeDelete(instance_);
	}
}

void RenderContextManager::Load(const LoadFileNames& fileNames) {
	auto isExist = renderData_.find(fileNames);

	if (isExist == renderData_.end()) {
		return;
	}

	renderData_.insert(std::make_pair(fileNames, std::make_unique<RenderContext<>>()));

	RenderContext<>& currentRenderContext = *renderData_[fileNames];

	Shader shader = LoadShader(fileNames.shaderName);

	currentRenderContext.pipeline = CreateGraphicsPipelines(shader);

	currentRenderContext.mesh = MeshLoader::LoadObj(fileNames.reourceFileName);

	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	descriptorHeap->BookingHeapPos(3u);
	descriptorHeap->CreateView(currentRenderContext.light);
	descriptorHeap->CreateView(currentRenderContext.shaderStruct);
	descriptorHeap->CreateView(currentRenderContext.wvpMatrix);

}

RenderContext<>* const RenderContextManager::Get(const LoadFileNames& fileNames)
{
	auto isExist = renderData_.find(fileNames);

	if (isExist == renderData_.end()) {
		Load(fileNames);
	}

	return renderData_[fileNames].get();
}

Shader RenderContextManager::LoadShader(const ShaderFileNames& shaderName)
{
	Shader result;

	ShaderManager* const shaderMaanger = ShaderManager::GetInstance();


	result.vertex = shaderMaanger->LoadVertexShader(shaderName.vsFileName);
	result.pixel = shaderMaanger->LoadPixelShader(shaderName.psFileName);
	result.geometory = shaderMaanger->LoadGeometoryShader(shaderName.gsFileName);
	result.hull = shaderMaanger->LoadHullShader(shaderName.hsFileName);
	result.domain = shaderMaanger->LoadDomainShader(shaderName.dsFileName);

	return result;
}

RenderSetting RenderContextManager::CreateGraphicsPipelines(Shader shader)
{
	RenderSetting result;

	auto descriptorHeap = CbvSrvUavHeap::GetInstance();

	std::array<D3D12_DESCRIPTOR_RANGE, 1> srvRange = {};
	srvRange[0].BaseShaderRegister = 0;
	srvRange[0].NumDescriptors = 2;
	srvRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	srvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	std::array<D3D12_DESCRIPTOR_RANGE, 1> texRange = {};
	texRange[0].BaseShaderRegister = 2;
	texRange[0].NumDescriptors = descriptorHeap->GetMaxTexture();
	texRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;


	std::array<D3D12_ROOT_PARAMETER, 3> rootPrams = {};
	rootPrams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootPrams[0].Descriptor.ShaderRegister = 0;
	rootPrams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[1].DescriptorTable.NumDescriptorRanges = UINT(srvRange.size());
	rootPrams[1].DescriptorTable.pDescriptorRanges = srvRange.data();
	rootPrams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[2].DescriptorTable.NumDescriptorRanges = UINT(texRange.size());
	rootPrams[2].DescriptorTable.pDescriptorRanges = texRange.data();
	rootPrams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	PipelineManager::CreateRootSgnature(rootPrams.data(), rootPrams.size(), true, false);
	PipelineManager::SetShader(shader);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetVertexInput("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	PipelineManager::SetVertexInput("BLENDINDICES", 0, DXGI_FORMAT_R32_UINT);

	for (size_t i = 0; i < size_t(RenderSetting::Type::kNum); i++) {
		result.drawCount[RenderSetting::Type(i)] = 0;

		size_t blendType = i < Pipeline::Blend::BlendTypeNum ? i : i - Pipeline::Blend::BlendTypeNum;

		PipelineManager::IsDepth(i < Pipeline::Blend::BlendTypeNum);
		PipelineManager::SetState(Pipeline::Blend(blendType), Pipeline::SolidState::Solid);
		result.pieplines[i] = PipelineManager::Create();
	}


	PipelineManager::StateReset();

	return result;
}
