#include "Pipeline.h"
#include <cassert>
#include <algorithm>
#include "Utils/ExecutionLog.h"
#include "Engine/Graphics/RootSignature/RootSignature.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Error/Error.h"

Pipeline::Pipeline():
	graphicsPipelineState_(),
	vertexInput_(),
	desc_()
{}

Pipeline::Pipeline(Pipeline&& right) noexcept {
	*this = std::move(right);
}

Pipeline& Pipeline::operator=(Pipeline&& right) noexcept {
	graphicsPipelineState_ = std::move(right.graphicsPipelineState_);
	desc_.shader = std::move(right.desc_.shader);

	return *this;
}

bool Pipeline::operator==(const Pipeline& right) const {
	return desc_ == right.desc_;
}
bool Pipeline::operator!=(const Pipeline& right) const {
	return !this->operator==(right);
}

void Pipeline::AddVertexInput(const VSInputData& vsInpuptData) {
	D3D12_INPUT_ELEMENT_DESC inputElementDescs{};

	inputElementDescs.SemanticIndex = vsInpuptData.semanticIndex;
	inputElementDescs.Format = vsInpuptData.format;
	inputElementDescs.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs.InputSlot = vsInpuptData.inputSlot;
	inputElementDescs.SemanticName = vsInpuptData.semanticName.c_str();

	vertexInput_.push_back(inputElementDescs);
}

void Pipeline::Create(
	const Desc& desc
) {
	desc_ = desc;
	assert(1 <= desc_.numRenderTarget and desc_.numRenderTarget < 8);

	for (auto& i : desc_.vsInputData) {
		AddVertexInput(i);
	}

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	if (vertexInput_.empty()) {
		inputLayoutDesc.pInputElementDescs = nullptr;
		inputLayoutDesc.NumElements = 0;
	}
	else {
		inputLayoutDesc.pInputElementDescs = vertexInput_.data();
		inputLayoutDesc.NumElements = UINT(vertexInput_.size());
	}

	// RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE(desc_.cullMode);
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE(desc_.solidState);
	rasterizerDesc.DepthClipEnable = true;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = desc_.rootSignature->Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;

	graphicsPipelineStateDesc.VS = {
			desc_.shader.vertex->GetBufferPointer(),
			desc_.shader.vertex->GetBufferSize()
	};
	graphicsPipelineStateDesc.PS = {
			desc_.shader.pixel->GetBufferPointer(),
			desc_.shader.pixel->GetBufferSize()
	};
	if (desc_.shader.hull && desc_.shader.domain) {
		graphicsPipelineStateDesc.HS = {
				desc_.shader.hull->GetBufferPointer(),
				desc_.shader.hull->GetBufferSize()
		};
		graphicsPipelineStateDesc.DS = {
				desc_.shader.domain->GetBufferPointer(),
				desc_.shader.domain->GetBufferSize()
		};
	}
	if (desc_.shader.geometory) {
		graphicsPipelineStateDesc.GS = {
				desc_.shader.geometory->GetBufferPointer(),
				desc_.shader.geometory->GetBufferSize()
		};
	}

	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = desc_.numRenderTarget;
	for (uint32_t i = 0; i < desc_.numRenderTarget; ++i) {
		graphicsPipelineStateDesc.RTVFormats[i] = desc_.rtvFormtat[i];
	}
	// 利用するトポロジ(形状)のタイプ
	graphicsPipelineStateDesc.PrimitiveTopologyType = desc_.topologyType;

	// どのように画面に打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleDesc.Quality = 0;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	if (desc_.isDepth) {
		graphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
		graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	}

	for (uint32_t i = 0; i < desc_.numRenderTarget; i++) {
		graphicsPipelineStateDesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		
		switch (desc_.blend[i])
		{
		case Pipeline::Blend::None:
		default:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = false;
			break;
		case Pipeline::Blend::Normal:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			break;
		case Pipeline::Blend::Add:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			break;
		case Pipeline::Blend::Sub:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_SUBTRACT;
			break;
		case Pipeline::Blend::Mul:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_SRC_COLOR;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			break;
		}

		graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
		graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
		graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	}

	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();
	HRESULT hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(graphicsPipelineState_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<Pipeline>("CreateGraphicsPipelineState failed", ErrorPlace);
	}
}

void Pipeline::CreateCubeMap(
	const Desc& desc
) {
	desc_ = desc;
	assert(1 <= desc_.numRenderTarget and desc_.numRenderTarget < 8);

	for (auto& i : desc_.vsInputData) {
		AddVertexInput(i);
	}

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = vertexInput_.data();
	inputLayoutDesc.NumElements = UINT(vertexInput_.size());

	// RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE(desc_.cullMode);
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE(desc_.solidState);
	rasterizerDesc.DepthClipEnable = true;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = desc_.rootSignature->Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;

	graphicsPipelineStateDesc.VS = {
			desc_.shader.vertex->GetBufferPointer(),
			desc_.shader.vertex->GetBufferSize()
	};
	graphicsPipelineStateDesc.PS = {
			desc_.shader.pixel->GetBufferPointer(),
			desc_.shader.pixel->GetBufferSize()
	};
	if (desc_.shader.hull && desc_.shader.domain) {
		graphicsPipelineStateDesc.HS = {
				desc_.shader.hull->GetBufferPointer(),
				desc_.shader.hull->GetBufferSize()
		};
		graphicsPipelineStateDesc.DS = {
				desc_.shader.domain->GetBufferPointer(),
				desc_.shader.domain->GetBufferSize()
		};
	}
	if (desc_.shader.geometory) {
		graphicsPipelineStateDesc.GS = {
				desc_.shader.geometory->GetBufferPointer(),
				desc_.shader.geometory->GetBufferSize()
		};
	}


	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = desc_.numRenderTarget;
	// 利用するトポロジ(形状)のタイプ
	graphicsPipelineStateDesc.PrimitiveTopologyType = desc_.topologyType;

	// どのように画面に打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleDesc.Quality = 0;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	desc_.isDepth = true;

	graphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
	graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	for (uint32_t i = 0; i < desc_.numRenderTarget; i++) {
		graphicsPipelineStateDesc.RTVFormats[i] = desc_.rtvFormtat[i];
		graphicsPipelineStateDesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		
		switch (desc_.blend[i])
		{
		case Pipeline::Blend::None:
		default:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = false;
			break;
		case Pipeline::Blend::Normal:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			break;
		case Pipeline::Blend::Add:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			break;
		case Pipeline::Blend::Sub:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_SUBTRACT;
			break;
		case Pipeline::Blend::Mul:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_SRC_COLOR;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			break;
		}

		graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
		graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
		graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	}

	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();
	HRESULT hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(graphicsPipelineState_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<Pipeline>("CreateGraphicsPipelineState failed", ErrorPlace);
	}
}

void Pipeline::Use() const {
	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();

	assert(graphicsPipelineState_);
	if (!graphicsPipelineState_) {
		throw Lamb::Error::Code<Pipeline>("GraphicsPipelineState is nullptr", ErrorPlace);
	}
	auto commandlist = DirectXCommand::GetMainCommandlist()->GetCommandList();
	commandlist->SetGraphicsRootSignature(desc_.rootSignature->Get());
	commandlist->SetPipelineState(graphicsPipelineState_.Get());

	switch (desc_.topologyType)
	{
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE:
		commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:
		commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH:
		commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		break;
	default:
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED:
	case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT:
		throw Lamb::Error::Code<Pipeline>("Cannot use this primitive topology type", ErrorPlace);
		break;
	}
}

bool Pipeline::IsSame(
	const Desc& desc
) {
	return desc_ == desc;
}

bool Pipeline::Desc::operator==(const Pipeline::Desc& right) const {
	return this->shader == right.shader
		and this->vsInputData == right.vsInputData
		and this->blend == right.blend
		and this->rtvFormtat == right.rtvFormtat
		and this->cullMode == right.cullMode
		and this->solidState == right.solidState
		and this->topologyType == right.topologyType
		and this->numRenderTarget == right.numRenderTarget
		and this->isDepth == right.isDepth;
}