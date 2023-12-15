//#include "WaterPipeline.h"
//#include <cassert>
//#include "Engine/Graphics/PipelineManager/PipelineManager.h"
//#include "Utils/Random/Random.h"
//#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
//#include "Engine/Core/DirectXCommand/DirectXCommand.h"
//
//void WaterPipeline::Use() {
//	assert(pipeline_);
//
//	pipeline_->Use();
//
//	randomVec_->x = Lamb::Random(0.0f, 1.0f);
//	randomVec_->y = Lamb::Random(0.0f, 1.0f);
//
//	*colorBuf_ = color;
//
//	*wvpMat_ = wvp;
//
//	ID3D12GraphicsCommandList* const commandList = DirectXCommand::GetInstance()->GetCommandList();
//
//	commandList->SetGraphicsRootDescriptorTable(1, wvpMat_.GetViewHandle());
//}
//
//void WaterPipeline::Init(const std::string& vsShader, const std::string& psShader) {
//	this->LoadShader(vsShader, psShader);
//
//	std::array<D3D12_DESCRIPTOR_RANGE, 1> renderRange = {};
//	renderRange[0].BaseShaderRegister = 0;
//	renderRange[0].NumDescriptors = 1;
//	renderRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//	renderRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//	std::array<D3D12_DESCRIPTOR_RANGE, 1> cbvRange = {};
//	cbvRange[0].BaseShaderRegister = 0;
//	cbvRange[0].NumDescriptors = 3;
//	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
//	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//	std::array<D3D12_ROOT_PARAMETER, 2> roootParamater = {};
//	roootParamater[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	roootParamater[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//	roootParamater[0].DescriptorTable.pDescriptorRanges = renderRange.data();
//	roootParamater[0].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(renderRange.size());
//
//	roootParamater[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	roootParamater[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
//	roootParamater[1].DescriptorTable.pDescriptorRanges = cbvRange.data();
//	roootParamater[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(cbvRange.size());
//
//	PipelineManager::CreateRootSgnature(roootParamater.data(), roootParamater.size(), true);
//
//	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
//	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
//
//	PipelineManager::SetShader(shader_);
//	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid);
//
//	PipelineManager::IsDepth(false);
//
//	PipelineManager::SetState(Pipeline::Blend::Normal, Pipeline::SolidState::Solid);
//	pipeline_ = PipelineManager::Create();
//
//	PipelineManager::StateReset();
//
//
//
//
//	static auto srvHeap = CbvSrvUavHeap::GetInstance();
//	srvHeap->BookingHeapPos(3u);
//	srvHeap->CreateConstBufferView(wvpMat_);
//	srvHeap->CreateConstBufferView(colorBuf_);
//	srvHeap->CreateConstBufferView(randomVec_);
//}