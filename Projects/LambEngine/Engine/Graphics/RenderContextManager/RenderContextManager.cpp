#include "RenderContextManager.h"
#include "Utils/SafeDelete.h"

#include "../Shader/ShaderManager/ShaderManager.h"

#include "../../Core/DescriptorHeap/CbvSrvUavHeap.h"

#include "../PipelineManager/PipelineManager.h"

Lamb::SafePtr<RenderContextManager> RenderContextManager::instance_ = nullptr;

RenderContextManager::~RenderContextManager()
{
}

RenderContextManager* const RenderContextManager::GetInstance() {
	return instance_.get();
}

void RenderContextManager::Initialize() {
	instance_.reset(new RenderContextManager());
	Lamb::AddLog("Initialize RenderContextManager succeeded");
}

void RenderContextManager::Finalize() {
	instance_.reset();
	Lamb::AddLog("Finalize RenderContextManager succeeded");
}

RenderSet* const RenderContextManager::Get(const LoadFileNames& fileNames)
{
	auto isExist = renderData_.find(fileNames);
	if (isExist != renderData_.end()) {
		return renderData_[fileNames].get();
	}
	else if (isExist == renderData_.end() and not isNowThreading_) {
		Load(fileNames);
		return renderData_[fileNames].get();
	}
	else {
		isExist = threadRenderData_.find(fileNames);
		if (isExist == threadRenderData_.end()) {
			Load(fileNames);
		}

		return threadRenderData_[fileNames].get();
	}
}

void RenderContextManager::SetIsNowThreading(bool isNowThreading) {
	isNowThreading_ = isNowThreading;

	if (not isNowThreading_) {
		for (auto& i : threadRenderData_) {
			renderData_.insert(std::make_pair(i.first, i.second.release()));
		}

		threadRenderData_.clear();
	}
}

std::pair<size_t, const std::list<RenderData*>&> RenderContextManager::CreateRenderList(BlendType blend)
{
	std::pair<size_t, const std::list<RenderData*>&> result = { 0llu, renderDataLists_[blend] };
	size_t count = 0;
	auto itr = renderDataLists_[blend].begin();
	
	for (auto& i : renderData_) {
		if (i.second->IsDraw(blend)) {
			if (itr == renderDataLists_[blend].end()) {
				break;
			}
			*itr = i.second->GetRenderData(blend);
			itr++;
			count++;
		}
	}

	result.first = count;

	return result;
}

void RenderContextManager::ResizeRenderList() {
	for (auto& i : renderDataLists_) {
		i.resize(renderData_.size());
	}
}

void RenderContextManager::Draw() {
	for (auto& i : renderData_) {
		i.second->Draw();
	}
}

void RenderContextManager::ResetDrawCount() {
	for (auto& i : renderData_) {
		i.second->ResetDrawCount();
	}
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

std::array<Pipeline*, BlendType::kNum> RenderContextManager::CreateGraphicsPipelines(Shader shader, uint32_t numRenderTarget)
{
	std::array<Pipeline*, BlendType::kNum> result = {nullptr};

	auto srvHeap = CbvSrvUavHeap::GetInstance();


	std::array<D3D12_DESCRIPTOR_RANGE, 1> cbvRange = {};
	cbvRange[0].NumDescriptors = 2;
	cbvRange[0].BaseShaderRegister = 0;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

	uint32_t baseShaderRegister = 0;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> srvRange = {};
	srvRange[0].NumDescriptors = 3;
	srvRange[0].BaseShaderRegister = baseShaderRegister;
	srvRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	srvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	baseShaderRegister = srvRange[0].NumDescriptors;

	std::array<D3D12_DESCRIPTOR_RANGE, 1> texRange = {};
	texRange[0].NumDescriptors = srvHeap->GetMaxTexture();
	texRange[0].BaseShaderRegister = baseShaderRegister;
	texRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;


	std::array<D3D12_ROOT_PARAMETER, 3> rootPrams = {};
	rootPrams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[0].DescriptorTable.NumDescriptorRanges = UINT(cbvRange.size());
	rootPrams[0].DescriptorTable.pDescriptorRanges = cbvRange.data();
	rootPrams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[1].DescriptorTable.NumDescriptorRanges = UINT(srvRange.size());
	rootPrams[1].DescriptorTable.pDescriptorRanges = srvRange.data();
	rootPrams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[2].DescriptorTable.NumDescriptorRanges = UINT(texRange.size());
	rootPrams[2].DescriptorTable.pDescriptorRanges = texRange.data();
	rootPrams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	RootSignature::Desc desc;
	desc.rootParameter = rootPrams.data();
	desc.rootParameterSize = rootPrams.size();
	desc.samplerDeacs.push_back(
		CreateLinearSampler()
	);


	auto pipelineManager = PipelineManager::GetInstance();
	Pipeline::Desc pipelineDesc;
	pipelineDesc.rootSignature = pipelineManager->CreateRootSgnature(desc, true);
	pipelineDesc.vsInputData.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT });
	pipelineDesc.vsInputData.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT });
	pipelineDesc.vsInputData.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT });
	pipelineDesc.vsInputData.push_back({ "BLENDINDICES", 0, DXGI_FORMAT_R32_UINT });
	pipelineDesc.shader = shader;
	pipelineDesc.isDepth = false;
	pipelineDesc.blend[0] = Pipeline::None;
	pipelineDesc.solidState = Pipeline::SolidState::Solid;
	pipelineDesc.cullMode = Pipeline::CullMode::Back;
	pipelineDesc.topologyType = (shader.hull != nullptr ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH : D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pipelineDesc.numRenderTarget = numRenderTarget;


	for (size_t i = 0; i < size_t(BlendType::kNum); i++) {
		size_t blendType = i < Pipeline::Blend::BlendTypeNum ? i : i - Pipeline::Blend::BlendTypeNum;

		pipelineDesc.isDepth = i < Pipeline::Blend::BlendTypeNum;
		for (uint32_t renderCount = 0; renderCount < pipelineDesc.numRenderTarget; renderCount++) {
			if (pipelineDesc.isDepth) {
				pipelineDesc.rtvFormtat[renderCount] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			else {
				pipelineDesc.rtvFormtat[renderCount] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			}
		}
		
		for (uint32_t blendIndex = 0; blendIndex < pipelineDesc.numRenderTarget; blendIndex++) {
			pipelineDesc.blend[blendIndex] = Pipeline::Blend(blendType);
		}
		pipelineManager->SetDesc(pipelineDesc);
		result[i] = pipelineManager->Create();
	}


	pipelineManager->StateReset();

	return result;
}

std::array<Pipeline*, BlendType::kNum> RenderContextManager::CreateSkinAnimationGraphicsPipelines(Shader shader, uint32_t numRenderTarget)
{
	std::array<Pipeline*, BlendType::kNum> result = { nullptr };

	auto srvHeap = CbvSrvUavHeap::GetInstance();


	std::array<D3D12_DESCRIPTOR_RANGE, 1> cbvRange = {};
	cbvRange[0].NumDescriptors = 2;
	cbvRange[0].BaseShaderRegister = 0;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

	std::array<D3D12_DESCRIPTOR_RANGE, 1> srvRange = {};
	srvRange[0].NumDescriptors = 3;
	srvRange[0].BaseShaderRegister = 0;
	srvRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	srvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	std::array<D3D12_DESCRIPTOR_RANGE, 1> skinRange = {};
	skinRange[0].NumDescriptors = 1;
	skinRange[0].BaseShaderRegister = 4;
	skinRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	skinRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	std::array<D3D12_DESCRIPTOR_RANGE, 1> texRange = {};
	texRange[0].NumDescriptors = srvHeap->GetMaxTexture();
	texRange[0].BaseShaderRegister = 3;
	texRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;



	std::array<D3D12_ROOT_PARAMETER, 4> rootPrams = {};
	rootPrams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[0].DescriptorTable.NumDescriptorRanges = UINT(cbvRange.size());
	rootPrams[0].DescriptorTable.pDescriptorRanges = cbvRange.data();
	rootPrams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[1].DescriptorTable.NumDescriptorRanges = UINT(srvRange.size());
	rootPrams[1].DescriptorTable.pDescriptorRanges = srvRange.data();
	rootPrams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[2].DescriptorTable.NumDescriptorRanges = UINT(skinRange.size());
	rootPrams[2].DescriptorTable.pDescriptorRanges = skinRange.data();
	rootPrams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	rootPrams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[3].DescriptorTable.NumDescriptorRanges = UINT(texRange.size());
	rootPrams[3].DescriptorTable.pDescriptorRanges = texRange.data();
	rootPrams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	RootSignature::Desc desc;
	desc.rootParameter = rootPrams.data();
	desc.rootParameterSize = rootPrams.size();
	desc.samplerDeacs.push_back(
		CreateLinearSampler()
	);

	auto pipelineManager = PipelineManager::GetInstance();
	Pipeline::Desc pipelineDesc;
	pipelineDesc.rootSignature = pipelineManager->CreateRootSgnature(desc, true);
	pipelineDesc.vsInputData.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT });
	pipelineDesc.vsInputData.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT });
	pipelineDesc.vsInputData.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT });
	pipelineDesc.vsInputData.push_back({ "BLENDINDICES", 0, DXGI_FORMAT_R32_UINT });

	pipelineDesc.vsInputData.push_back({ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1 });
	pipelineDesc.vsInputData.push_back({ "WEIGHT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1 });
	pipelineDesc.vsInputData.push_back({ "INDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 1 });
	pipelineDesc.vsInputData.push_back({ "INDEX", 1, DXGI_FORMAT_R32G32B32A32_SINT, 1 });
	pipelineDesc.shader = shader;
	pipelineDesc.isDepth = false;
	pipelineDesc.blend[0] = Pipeline::None;
	pipelineDesc.solidState = Pipeline::SolidState::Solid;
	pipelineDesc.cullMode = Pipeline::CullMode::None;
	pipelineDesc.topologyType = (shader.hull != nullptr ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH : D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pipelineDesc.numRenderTarget = numRenderTarget;

	for (size_t i = 0; i < size_t(BlendType::kNum); i++) {
		size_t blendType = i < Pipeline::Blend::BlendTypeNum ? i : i - Pipeline::Blend::BlendTypeNum;

		pipelineDesc.isDepth = i < Pipeline::Blend::BlendTypeNum;

		for (uint32_t renderCount = 0; renderCount < pipelineDesc.numRenderTarget; renderCount++) {
			if (pipelineDesc.isDepth) {
				pipelineDesc.rtvFormtat[renderCount] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			else {
				pipelineDesc.rtvFormtat[renderCount] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			}
		}

		for (uint32_t blendIndex = 0; blendIndex < pipelineDesc.numRenderTarget; blendIndex++) {
			pipelineDesc.blend[blendIndex] = Pipeline::Blend(blendType);
		}
		pipelineManager->SetDesc(pipelineDesc);
		result[i] = pipelineManager->Create();
	}


	pipelineManager->StateReset();

	return result;
}
