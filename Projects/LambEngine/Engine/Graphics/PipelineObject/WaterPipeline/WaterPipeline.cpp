#include "WaterPipeline.h"
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


void WaterPipeline::Update() {
#ifdef _DEBUG
	ImGui::Begin("Water");
	ImGui::DragFloat("densityScale", &(*densityScale_), 0.01f);
	ImGui::DragFloat2("ゆらゆらの速度", &speed_.x, 0.001f);
	ImGui::End();
#endif // _DEBUG


	randomVec_->x += speed_.x * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);
	randomVec_->y += speed_.y * Lamb::DeltaTime() * Lamb::Random(0.8f, 1.2f);

	*colorBuf_ = color;

	wvpMat_->worldMat = worldMat;
	wvpMat_->viewProjection = viewProjection;

	normalVector_->normal = Vector3{ 0.0f,1.0f,0.0f };
	normalVector_->tangent = Vector3{ 0.0f,0.0f,1.0f };

	light_->ligColor = Vector3::kIdentity * 15.0f;
	light_->ligDirection = Vector3{ 1.0f,-1.0f,0.0f }.Normalize();
}

void WaterPipeline::Use(Pipeline::Blend blendType, bool isDepth) {
	if (isDepth) {
		pipelines_[blendType]->Use();
	}
	else {
		pipelinesNoDepth_[blendType]->Use();
	}
	auto* const commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();

	render_->UseThisRenderTargetShaderResource();
	caustics_->Use(1);
	commandList->SetGraphicsRootDescriptorTable(2, wvpMat_.GetHandleGPU());
}

void WaterPipeline::Init(
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
	cbvRange[0].NumDescriptors = 6;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	std::array<D3D12_ROOT_PARAMETER, 3> rootParameter = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = renderRange.data();
	rootParameter[0].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(renderRange.size());

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = causticsRange.data();
	rootParameter[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(causticsRange.size());

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = cbvRange.data();
	rootParameter[2].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(cbvRange.size());

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

	pipelineManager->StateReset();


	CbvSrvUavHeap* const srvHeap = CbvSrvUavHeap::GetInstance();
	TextureManager::GetInstance()->LoadTexture("./Resources/Common/Water/caustics_01.bmp");
	caustics_ = TextureManager::GetInstance()->GetTexture("./Resources/Common/Water/caustics_01.bmp");

	srvHeap->BookingHeapPos(7u);
	srvHeap->CreateView(*render_);
	srvHeap->CreateView(wvpMat_);
	srvHeap->CreateView(colorBuf_);
	srvHeap->CreateView(normalVector_);
	srvHeap->CreateView(randomVec_);
	srvHeap->CreateView(light_);
	srvHeap->CreateView(densityScale_);

	*densityScale_ = 1.0f;

	randomVec_->x = Lamb::Random(0.0f, 1.0f);
	randomVec_->y = Lamb::Random(0.0f, 1.0f);

	speed_ = { 0.006f, 0.006f };
}

WaterPipeline::~WaterPipeline() {
	if (render_) {
		auto* const srvHeap = CbvSrvUavHeap::GetInstance();
		srvHeap->ReleaseView(render_->GetHandleUINT());
		srvHeap->ReleaseView(wvpMat_.GetHandleUINT());
		srvHeap->ReleaseView(colorBuf_.GetHandleUINT());
		srvHeap->ReleaseView(randomVec_.GetHandleUINT());
		srvHeap->ReleaseView(normalVector_.GetHandleUINT());
		srvHeap->ReleaseView(light_.GetHandleUINT());
		srvHeap->ReleaseView(densityScale_.GetHandleUINT());
		if (caustics_) {
			caustics_->Unload();
		}
	}

	render_.reset();
}