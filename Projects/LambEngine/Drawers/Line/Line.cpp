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

std::unique_ptr<StructuredBuffer<Mat4x4>> Line::wvpMat_;

void Line::Initialize() {
	ShaderManager* const shaderManager = ShaderManager::GetInstance();
	shader_.vertex_ = shaderManager->LoadVertexShader("./Resources/Shaders/LineShader/Line.VS.hlsl");
	shader_.pixel_ = shaderManager->LoadPixelShader("./Resources/Shaders/LineShader/Line.PS.hlsl");

	D3D12_DESCRIPTOR_RANGE range = {};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
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
	PipelineManager::SetVertexInput("BLENDINDICES", 0, DXGI_FORMAT_R32_UINT);
	PipelineManager::SetShader(shader_);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid, Pipeline::CullMode::None, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	pipline_ = PipelineManager::Create();
	PipelineManager::StateReset();

	wvpMat_ = std::make_unique<StructuredBuffer<Mat4x4>>(kDrawMaxNumber_);

	CbvSrvUavHeap* const heap = CbvSrvUavHeap::GetInstance();
	heap->BookingHeapPos(1);
	heap->CreateStructuredBufferView(*wvpMat_);

	vertexBuffer_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(VertexData) * kVertexNum * kDrawMaxNumber_);
	vertexView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexView_.SizeInBytes = sizeof(VertexData) * kVertexNum * kDrawMaxNumber_;
	vertexView_.StrideInBytes = sizeof(VertexData);
}

void Line::Finalize() {
	CbvSrvUavHeap* const heap = CbvSrvUavHeap::GetInstance();
	heap->ReleaseView(wvpMat_->GetViewHandleUINT());
	wvpMat_.reset();

	if (vertexBuffer_) {
		vertexBuffer_->Release();
		vertexBuffer_.Reset();
	}
}

void Line::ResetDrawCount() {
	indexCount_ = 0u;
}

Line::Line() : 
	start(),
	end()
{
	
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

	// 頂点バッファマップ
	VertexData* vertexMap = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap));

	size_t indexVertex = kVertexNum * indexCount_;

	auto&& colorFloat = UintToVector4(color);
	vertexMap[indexVertex] = { Vector4(start, 1.0f), colorFloat, indexCount_ };
	vertexMap[indexVertex + 1] = { Vector4(end, 1.0f),   colorFloat, indexCount_ };

	vertexBuffer_->Unmap(0, nullptr);

	(*wvpMat_)[indexCount_] = viewProjection;

	pipline_->Use();
	CbvSrvUavHeap* const heap = CbvSrvUavHeap::GetInstance();
	heap->Use(wvpMat_->GetViewHandleUINT(), 0);
	auto commandList = DirectXCommand::GetInstance()->GetCommandList();
	commandList->IASetVertexBuffers(0, 1, &vertexView_);
	commandList->DrawInstanced(kVertexNum, 1, static_cast<uint32_t>(indexVertex), 0);

	indexCount_++;
}