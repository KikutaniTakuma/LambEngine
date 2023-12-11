#include "Line.h"
#include <algorithm>
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "imgui.h"

uint32_t Line::indexCount_ = 0u;
Shader Line::shader_ = {};
Pipeline* Line::pipline_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12Resource> Line::vertexBuffer_;
// 頂点バッファビュー
D3D12_VERTEX_BUFFER_VIEW Line::vertexView_;

std::unique_ptr<StructuredBuffer<Mat4x4>> Line::wvpMat_;
std::unique_ptr<StructuredBuffer<Vector4>> Line::color_;

void Line::Initialize() {
	ShaderManager* const shaderManager = ShaderManager::GetInstance();
	shader_.vertex_ = shaderManager->LoadVertexShader("./Resources/Shaders/LineShader/Line.VS.hlsl");
	shader_.pixel_ = shaderManager->LoadPixelShader("./Resources/Shaders/LineShader/Line.PS.hlsl");

	D3D12_DESCRIPTOR_RANGE range = {};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 2;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER paramater = {};
	paramater.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	paramater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	paramater.DescriptorTable.pDescriptorRanges = &range;
	paramater.DescriptorTable.NumDescriptorRanges = 1;

	PipelineManager::CreateRootSgnature(&paramater, 1, false);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetShader(shader_);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid, Pipeline::CullMode::None, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	pipline_ = PipelineManager::Create();
	PipelineManager::StateReset();

	wvpMat_ = std::make_unique<StructuredBuffer<Mat4x4>>(kDrawMaxNumber_);
	color_ = std::make_unique<StructuredBuffer<Vector4>>(kDrawMaxNumber_);

	CbvSrvUavHeap* const heap = CbvSrvUavHeap::GetInstance();
	heap->BookingHeapPos(2);
	heap->CreateStructuredBufferView(*wvpMat_);
	heap->CreateStructuredBufferView(*color_);

	vertexBuffer_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(Vector4) * kVertexNum);
	vertexView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexView_.SizeInBytes = sizeof(Vector4) * kVertexNum;
	vertexView_.StrideInBytes = sizeof(Vector4);

	Vector4* vertexMap = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap));
	vertexMap[0] = Vector4::kZero + Vector4::kWIndentity;
	vertexMap[1] = Vector4::kXIndentity + Vector4::kWIndentity;
	vertexBuffer_->Unmap(0, nullptr);
}

void Line::Finalize() {
	CbvSrvUavHeap* const heap = CbvSrvUavHeap::GetInstance();
	heap->ReleaseView(wvpMat_->GetViewHandleUINT());
	heap->ReleaseView(color_->GetViewHandleUINT());
	wvpMat_.reset();
	color_.reset();

	if (vertexBuffer_) {
		vertexBuffer_->Release();
		vertexBuffer_.Reset();
	}
}

void Line::ResetDrawCount() {
	indexCount_ = 0u;
}

void Line::AllDraw() {
	if (indexCount_ == 0u) {
		return;
	}

	pipline_->Use();
	CbvSrvUavHeap* const heap = CbvSrvUavHeap::GetInstance();
	heap->Use(wvpMat_->GetViewHandleUINT(), 0);
	auto commandList = DirectXCommand::GetInstance()->GetCommandList();
	commandList->IASetVertexBuffers(0, 1, &vertexView_);
	commandList->DrawInstanced(kVertexNum, indexCount_, 0, 0);

	ResetDrawCount();
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

void Line::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("start", &start.x, 0.01f);
	ImGui::DragFloat3("end", &end.x, 0.01f);
	ImGui::End();
#endif // _DEBUG
}

void Line::Draw(const Mat4x4& viewProjection, uint32_t color) {
	assert(indexCount_ < kDrawMaxNumber_);
	if (!(indexCount_ < kDrawMaxNumber_)) {
		Lamb::ErrorLog("Over Draw index", "Draw", "Line");
		return;
	}

	auto&& colorFloat = UintToVector4(color);

	(*color_)[indexCount_] = colorFloat;

	Vector3 scale;
	scale.x = (end - start).Length();
	Vector3 to = (end - start).Normalize();
	Vector3 translate = start;

	(*wvpMat_)[indexCount_] = Mat4x4::MakeAffin(scale, Vector3::kXIndentity, to, translate) * viewProjection;

	indexCount_++;
}