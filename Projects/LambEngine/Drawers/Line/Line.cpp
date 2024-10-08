#include "Line.h"
#include <algorithm>
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Error/Error.h"
#include "imgui.h"
#include <numbers>

#include "Utils/EngineInfo.h"

uint32_t Line::nodepthDrawCount_ = 0u;
uint32_t Line::depthDrawCount_ = 0u;
Shader Line::shader_ = {};
Lamb::SafePtr<class Pipeline> Line::depthPipeline_ = nullptr;
Lamb::SafePtr<Pipeline> Line::nodepthPipeline_ = nullptr;
std::array<std::unique_ptr<StructuredBuffer<Line::VertxData>>, DirectXSwapChain::kBackBufferNumber> Line::nodepthVertData_;
std::array<std::unique_ptr<StructuredBuffer<Line::VertxData>>, DirectXSwapChain::kBackBufferNumber> Line::depthVertData_;

void Line::Initialize() {
	Lamb::SafePtr shaderManager = ShaderManager::GetInstance();
	shader_.vertex = shaderManager->LoadVertexShader("./Shaders/LineShader/Line.VS.hlsl");
	shader_.pixel = shaderManager->LoadPixelShader("./Shaders/LineShader/Line.PS.hlsl");

	D3D12_DESCRIPTOR_RANGE range = {};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 1;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER paramater = {};
	paramater.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	paramater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	paramater.DescriptorTable.pDescriptorRanges = &range;
	paramater.DescriptorTable.NumDescriptorRanges = 1;

	RootSignature::Desc desc;
	desc.rootParameter = &paramater;
	desc.rootParameterSize = 1;
	desc.samplerDeacs.push_back(
		CreateLinearSampler()
	);

	auto pipelineManager = PipelineManager::GetInstance();
	Pipeline::Desc pipelineDesc;
	pipelineDesc.rootSignature = pipelineManager->CreateRootSgnature(desc, false);
	pipelineDesc.vsInputData.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT });
	pipelineDesc.shader = shader_;
	pipelineDesc.blend[0] = Pipeline::None;
	pipelineDesc.solidState = Pipeline::SolidState::Solid;
	pipelineDesc.cullMode = Pipeline::CullMode::None;
	pipelineDesc.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	pipelineDesc.numRenderTarget = 1;

	pipelineDesc.rtvFormtat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	pipelineDesc.isDepth = true;
	pipelineManager->SetDesc(pipelineDesc);
	depthPipeline_ = pipelineManager->Create();

	pipelineDesc.rtvFormtat[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	pipelineDesc.isDepth = false;
	pipelineManager->SetDesc(pipelineDesc);
	nodepthPipeline_ = pipelineManager->Create();

	pipelineManager->StateReset();

	std::for_each(
		nodepthVertData_.begin(), 
		nodepthVertData_.end(), 
		[](auto& n) {
			n = std::make_unique<StructuredBuffer<VertxData>>(); 
			n->Create(Line::kDrawMaxNumber_);
		}
	);
	std::for_each(
		depthVertData_.begin(),
		depthVertData_.end(),
		[](auto& n) {
			n = std::make_unique<StructuredBuffer<VertxData>>();
			n->Create(Line::kDrawMaxNumber_);
		}
	);

	Lamb::SafePtr heap = CbvSrvUavHeap::GetInstance();

	for (uint32_t i = 0; i < DirectXSwapChain::kBackBufferNumber; ++i) {
		heap->BookingHeapPos(DirectXSwapChain::kBackBufferNumber);
		heap->CreateView(*nodepthVertData_[i]);
		heap->CreateView(*depthVertData_[i]);
	}
}

void Line::Finalize() {
	CbvSrvUavHeap* const heap = CbvSrvUavHeap::GetInstance();
	for (uint32_t i = 0; i < DirectXSwapChain::kBackBufferNumber; ++i) {
		heap->BookingHeapPos(DirectXSwapChain::kBackBufferNumber);
		heap->ReleaseView(nodepthVertData_[i]->GetHandleUINT());
		heap->ReleaseView(depthVertData_[i]->GetHandleUINT());
	}
	std::for_each(
		nodepthVertData_.begin(),
		nodepthVertData_.end(),
		[](auto& n) {
			n.reset();
		}
	);
	std::for_each(
		depthVertData_.begin(),
		depthVertData_.end(),
		[](auto& n) {
			n.reset();
		}
	);
}

void Line::AllDraw(bool isDepth) {
	if (isDepth) {
		if (depthDrawCount_ == 0u) {
			return;
		}
		
		depthPipeline_->Use();
		Lamb::SafePtr heap = CbvSrvUavHeap::GetInstance();
		heap->Use(depthVertData_[Lamb::GetBufferINdex()]->GetHandleUINT(), 0);
		auto commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();
		commandList->DrawInstanced(kVertexNum, depthDrawCount_, 0, 0);

		depthDrawCount_ = 0u;
	}
	else {
		if (nodepthDrawCount_ == 0u) {
			return;
		}

		nodepthPipeline_->Use();
		Lamb::SafePtr heap = CbvSrvUavHeap::GetInstance();
		heap->Use(nodepthVertData_[Lamb::GetBufferINdex()]->GetHandleUINT(), 0);
		auto commandList = DirectXCommand::GetMainCommandlist()->GetCommandList();
		commandList->DrawInstanced(kVertexNum, nodepthDrawCount_, 0, 0);

		nodepthDrawCount_ = 0u;
	}
}

Line::Line():
	start{},
	end{},
	color(std::numeric_limits<uint32_t>::max())
{}

void Line::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef USE_DEBUG_CODE
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("start", start.data(), 0.01f);
	ImGui::DragFloat3("end", end.data(), 0.01f);
	ImGui::End();
#endif // USE_DEBUG_CODE
}

void Line::Draw(const Mat4x4& viewProjection, bool isDepth) {
	if (isDepth) {
		assert(depthDrawCount_ < kDrawMaxNumber_);
		if (!(depthDrawCount_ < kDrawMaxNumber_)) {
			Lamb::Error::Code<Line>("Over Draw index", ErrorPlace);
		}

		auto&& colorFloat = UintToVector4(color);

		(*depthVertData_[Lamb::GetBufferINdex()])[depthDrawCount_].color = colorFloat;

		Vector3 scale;
		scale.x = (end - start).Length();
		Vector3 to = (end - start).Normalize();
		Vector3 translate = start;

		(*depthVertData_[Lamb::GetBufferINdex()])[depthDrawCount_].wvp = Mat4x4::MakeAffin(scale, Vector3::kXIdentity, to, translate) * viewProjection;

		depthDrawCount_++;
	}
	else {
		assert(nodepthDrawCount_ < kDrawMaxNumber_);
		if (!(nodepthDrawCount_ < kDrawMaxNumber_)) {
			Lamb::Error::Code<Line>("Over Draw index", ErrorPlace);
		}

		auto&& colorFloat = UintToVector4(color);

		(*nodepthVertData_[Lamb::GetBufferINdex()])[nodepthDrawCount_].color = colorFloat;

		Vector3 scale;
		scale.x = (end - start).Length();
		Vector3 to = (end - start).Normalize();
		Vector3 translate = start;

		(*nodepthVertData_[Lamb::GetBufferINdex()])[nodepthDrawCount_].wvp = Mat4x4::MakeAffin(scale, Vector3::kXIdentity, to, translate) * viewProjection;

		nodepthDrawCount_++;
	}
}

void Line::Draw(
	const Vector3& start, 
	const Vector3& end, 
	const Mat4x4& viewProjection, 
	uint32_t color,
	bool isDepth
) {
	if (isDepth) {
		assert(depthDrawCount_ < kDrawMaxNumber_);
		if (!(depthDrawCount_ < kDrawMaxNumber_)) {
			throw Lamb::Error::Code<Line>("Over Draw index", ErrorPlace);
		}

		auto&& colorFloat = UintToVector4(color);

		(*depthVertData_[Lamb::GetBufferINdex()])[depthDrawCount_].color = colorFloat;

		Vector3 scale;
		scale.x = (end - start).Length();
		Vector3 to = (end - start).Normalize();
		Vector3 translate = start;

		(*depthVertData_[Lamb::GetBufferINdex()])[depthDrawCount_].wvp = Mat4x4::MakeAffin(scale, Vector3::kXIdentity, to, translate) * viewProjection;

		depthDrawCount_++;
	}
	else {
		assert(nodepthDrawCount_ < kDrawMaxNumber_);
		if (!(nodepthDrawCount_ < kDrawMaxNumber_)) {
			throw Lamb::Error::Code<Line>("Over Draw index", ErrorPlace);
		}

		auto&& colorFloat = UintToVector4(color);

		(*nodepthVertData_[Lamb::GetBufferINdex()])[nodepthDrawCount_].color = colorFloat;

		Vector3 scale;
		scale.x = (end - start).Length();
		Vector3 to = (end - start).Normalize();
		Vector3 translate = start;

		(*nodepthVertData_[Lamb::GetBufferINdex()])[nodepthDrawCount_].wvp = Mat4x4::MakeAffin(scale, Vector3::kXIdentity, to, translate) * viewProjection;

		nodepthDrawCount_++;
	}
}