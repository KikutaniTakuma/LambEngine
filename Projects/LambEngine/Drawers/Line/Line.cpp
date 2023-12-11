#include "Line.h"
#include <algorithm>
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"

uint32_t Line::indexCount_ = 0u;
Shader Line::shader_ = {};
Pipeline* Line::pipline_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> Line::vertexBuffer_;
// 頂点バッファビュー
D3D12_VERTEX_BUFFER_VIEW Line::vertexView_;

void Line::Initialize() {
	ShaderManager* const shaderManager = ShaderManager::GetInstance();
	shader_.vertex_ = shaderManager->LoadVertexShader("./Resources/Shaders/LineShader/Line.VS.hlsl");
	shader_.pixel_ = shaderManager->LoadPixelShader("./Resources/Shaders/LineShader/Line.PS.hlsl");

	D3D12_DESCRIPTOR_RANGE range = {};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 1;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER paramater = {};
	paramater.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	paramater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	paramater.DescriptorTable.pDescriptorRanges = &range;
	paramater.DescriptorTable.NumDescriptorRanges = 1;

	PipelineManager::CreateRootSgnature(&paramater, 1, false);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetVertexInput("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetShader(shader_);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid, Pipeline::CullMode::None, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	pipline_ = PipelineManager::Create();
	PipelineManager::StateReset();

	vertexBuffer_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(VertexData) * kVertexNum * kDrawMaxNumber_);
	vertexView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexView_.SizeInBytes = sizeof(VertexData) * kVertexNum * kDrawMaxNumber_;
	vertexView_.StrideInBytes = sizeof(VertexData);
}

void Line::Finalize() {
	if (vertexBuffer_) {
		vertexBuffer_->Release();
		vertexBuffer_.Reset();
	}
}

void Line::ResetDrawCount() {
	indexCount_ = 0u;
}

Line::Line() : 
	vertexMap_(nullptr),
	heap_(),
	wvpMat_(),
	start(),
	end()
{
	heap_ = CbvSrvUavHeap::GetInstance();
	heap_->BookingHeapPos(1);
	heap_->CreateConstBufferView(wvpMat_);
}

Line::Line(const Line& right):
	Line()
{
	*this = right;
}
Line::Line(Line&& right) noexcept :
	Line()
{
	*this = std::move(right);
}

Line& Line::operator=(const Line& right) {
	start = right.start;
	end = right.end;

	return *this;
}
Line& Line::operator=(Line&& right)noexcept {
	start = std::move(right.start);
	end = std::move(right.end);

	return *this;
}

Line::~Line() {
	heap_->ReleaseView(wvpMat_.GetViewHandleUINT());
	if (vertexBuffer_) {
		vertexBuffer_->Release();
		vertexBuffer_.Reset();
	}
}

void Line::Draw(const Mat4x4& viewProjection, uint32_t color) {
	assert(indexCount_ < kDrawMaxNumber_);
	if (!(indexCount_ < kDrawMaxNumber_)) {
		Lamb::ErrorLog("Over Draw index", "Draw", "Line");
		return;
	}

	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap_));

	size_t indexVertex = kVertexNum * indexCount_;

	auto&& colorFloat = UintToVector4(color);
	vertexMap_[indexVertex] = { Vector4(start, 1.0f), colorFloat };
	vertexMap_[indexVertex + 1] = { Vector4(end, 1.0f),   colorFloat };

	vertexBuffer_->Unmap(0, nullptr);

	*wvpMat_ = viewProjection;

	pipline_->Use();
	heap_->Use(wvpMat_.GetViewHandleUINT(), 0);
	auto commandList = DirectXCommand::GetInstance()->GetCommandList();
	commandList->IASetVertexBuffers(0, 1, &vertexView_);
	commandList->DrawInstanced(kVertexNum, 1, static_cast<uint32_t>(indexVertex), 0);

	indexCount_++;
}