#include "Mesh.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "../../TextureManager/TextureManager.h"
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "../MeshManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Error/Error.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include <deque>
#include <algorithm>
#include <filesystem>

#include "../../MeshLoader/MeshLoader.h"

Mesh::Mesh() :
	texs_{},
	isLoad_(false),
	isObjLoad_(false),
	wvpMats_{256u},
	dirLig_{},
	color_{},
	drawCount_{0u}
{
	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	descriptorHeap->BookingHeapPos(3u);
	descriptorHeap->CreateView(wvpMats_);
	descriptorHeap->CreateView(dirLig_);
	descriptorHeap->CreateView(color_);
}

Mesh::~Mesh() {
	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	descriptorHeap->ReleaseView(wvpMats_.GetHandleUINT());
	descriptorHeap->ReleaseView(dirLig_.GetHandleUINT());
	descriptorHeap->ReleaseView(color_.GetHandleUINT());
}

void Mesh::LoadObj(const std::string& objFileName) {
	if (!isLoad_) {
		objFileName_ = objFileName;
		resource_ = MeshLoader::LoadModel(objFileName_, texs_);

		isLoad_ = true;
		isObjLoad_ = true;
	}
}

void Mesh::ReleaseResource() {
	for (auto& i : resource_) {
		if (i.second.resource.first) {
			i.second.resource.first->Release();
			i.second.resource.first = nullptr;
		}
	}
}

std::unordered_map<std::string, Mesh::CopyData> Mesh::CopyBuffer() const {
	std::unordered_map<std::string, Mesh::CopyData> copyData;
	for (auto& i : resource_) {
		copyData[i.first];
		copyData[i.first] = i.second;
	}
	return copyData;
}

void Mesh::ResetDrawCount() {
	drawCount_ = 0u;
}

void Mesh::ResetBufferSize() {
	wvpMats_.Resize(1u);
}

void Mesh::ResizeBuffers() {
	if (wvpMats_.Size() <= drawCount_) {
		uint32_t nextBufferSize = wvpMats_.Size() * 2u;
		wvpMats_.Resize(nextBufferSize);
	}
}

void Mesh::Use(
	const Mat4x4& wvpMat,
	const Mat4x4& viewOrojection,
	const Mesh::Light& light,
	const Vector4& color
) {
	ResizeBuffers();

	wvpMats_[drawCount_].worldMat = wvpMat;
	wvpMats_[drawCount_].viewProjectoionMat = viewOrojection;
	*dirLig_ = light;
	*color_ = color;

	drawCount_++;
}

void Mesh::Draw() {
	if (0u < drawCount_) {
		auto commandList = DirectXCommand::GetInstance()->GetCommandList();

		if (!pipeline_) {
			throw Lamb::Error::Code<Mesh>("pipeline is nullptr", __func__);
		}

		for (auto& i : resource_) {
			pipeline_->Use();
			i.second.tex->Use(0);

			commandList->SetGraphicsRootDescriptorTable(1, wvpMats_.GetHandleGPU());
			commandList->SetGraphicsRootDescriptorTable(2, dirLig_.GetHandleGPU());

			commandList->IASetVertexBuffers(0, 1, &i.second.resource.second);
			commandList->DrawInstanced(i.second.vertNum, drawCount_, 0, 0);
		}
	}
}

void Mesh::ChangeTexture(const std::string& useMtlName, const std::string& texName) {
	resource_[useMtlName].tex = TextureManager::GetInstance()->LoadTexture(texName);
	assert(resource_[useMtlName].tex->GetFileName() == texName);
}

void Mesh::ChangeTexture(const std::string& useMtlName, Texture* tex) {
	assert(tex != nullptr);
	resource_[useMtlName].tex = tex;
}

Shader Mesh::shader_ = {};

Pipeline* Mesh::pipeline_ = {};
bool Mesh::loadShaderFlg_ = false;
bool Mesh::createGPFlg_ = false;

void Mesh::Initialize(
	const std::string& vertex,
	const std::string& pixel,
	const std::string& geometory,
	const std::string& hull,
	const std::string& domain
) {
	LoadShader(vertex, pixel, geometory, hull, domain);
	CreateGraphicsPipeline();
}

void Mesh::LoadShader(
	const std::string& vertex,
	const std::string& pixel,
	const std::string& geometory,
	const std::string& hull,
	const std::string& domain
) {
	static ShaderManager* const shaderManager = shaderManager->GetInstance();
	if (!loadShaderFlg_) {
		shader_.vertex = shaderManager->LoadVertexShader(vertex);
		shader_.pixel = shaderManager->LoadPixelShader(pixel);
		if (geometory.size() != 0LLU) {
			shader_.geometory = shaderManager->LoadGeometoryShader(geometory);
		}
		if (hull.size() != 0LLU && geometory.size() != 0LLU) {
			shader_.hull = shaderManager->LoadHullShader(hull);
			shader_.domain = shaderManager->LoadHullShader(domain);
		}
		loadShaderFlg_ = true;
	}
}

void Mesh::CreateGraphicsPipeline() {
	if (loadShaderFlg_) {
		std::array<D3D12_DESCRIPTOR_RANGE, 1> rangeTex = {};
		rangeTex[0].NumDescriptors = 1;
		rangeTex[0].BaseShaderRegister = 0;
		rangeTex[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		rangeTex[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		std::array<D3D12_DESCRIPTOR_RANGE, 1> rangeWvp = {};
		rangeWvp[0].NumDescriptors = 1;
		rangeWvp[0].BaseShaderRegister = 1;
		rangeWvp[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		rangeWvp[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		std::array<D3D12_DESCRIPTOR_RANGE, 1> rangeCBV = {};
		rangeCBV[0].NumDescriptors = 2;
		rangeCBV[0].BaseShaderRegister = 0;
		rangeCBV[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		rangeCBV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;


		std::array<D3D12_ROOT_PARAMETER, 3> paramates = {};
		paramates[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[0].DescriptorTable.pDescriptorRanges = rangeTex.data();
		paramates[0].DescriptorTable.NumDescriptorRanges = UINT(rangeTex.size());

		paramates[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[1].DescriptorTable.pDescriptorRanges = rangeWvp.data();
		paramates[1].DescriptorTable.NumDescriptorRanges = UINT(rangeWvp.size());

		paramates[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[2].DescriptorTable.pDescriptorRanges = rangeCBV.data();
		paramates[2].DescriptorTable.NumDescriptorRanges = UINT(rangeCBV.size());

		PipelineManager::CreateRootSgnature(paramates.data(), paramates.size(), true);

		PipelineManager::SetShader(shader_);

		PipelineManager::SetVertexInput("POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT);
		PipelineManager::SetVertexInput("NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT);
		PipelineManager::SetVertexInput("TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT);

		PipelineManager::IsDepth();

		PipelineManager::SetState(Pipeline::Blend::None, Pipeline::SolidState::Solid);

		pipeline_ = PipelineManager::Create();

		PipelineManager::StateReset();

		createGPFlg_ = true;
	}
}