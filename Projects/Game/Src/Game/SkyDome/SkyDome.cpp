#include "SkyDome.h"
#include "./Camera/Camera.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"

void SkyDome::Initialize()
{
	mesh_ = MeshManager::GetInstance()->LoadObj("./Resources/skydome/skydome.obj");

	scale = Vector3::kIdentity * 300.0f;

	pipeline_ = CreatePipeline();

	CbvSrvUavHeap* const cbvSrvUavHeap = CbvSrvUavHeap::GetInstance();

	cbvSrvUavHeap->BookingHeapPos(2u);
	cbvSrvUavHeap->CreateView(wvpData_);
	cbvSrvUavHeap->CreateView(rayleighScattering_);

	tex_ = TextureManager::GetInstance()->GetWhiteTex();

	rayleighScattering_->light.color = Vector4::kIdentity;
	rayleighScattering_->light.direction = -Vector3::kYIdentity;
	rayleighScattering_->light.pos = Vector3::kYIdentity * 1000.0f;
	rayleighScattering_->light.intensity = 1.0f / static_cast<float>(1e-5);

	// 屈折率
	rayleighScattering_->air.refractiveIndex = 1.000277f;
	// 単位体積当たりの分子数
	rayleighScattering_->air.moleculesNum = static_cast<float>(2.547e25);
	// RGBの各波長
	rayleighScattering_->air.wavelengthR = static_cast<float>(650e-9);
	rayleighScattering_->air.wavelengthG = static_cast<float>(510e-9);
	rayleighScattering_->air.wavelengthB = static_cast<float>(475e-9);

}

void SkyDome::Finalize()
{
	CbvSrvUavHeap* const cbvSrvUavHeap = CbvSrvUavHeap::GetInstance();
	cbvSrvUavHeap->ReleaseView(rayleighScattering_.GetHandleUINT());
	cbvSrvUavHeap->ReleaseView(wvpData_.GetHandleUINT());
}

void SkyDome::Debug([[maybe_unused]]const std::string& guiName){

}

void SkyDome::Upadate()
{
	wvpData_->worldMat = Mat4x4::MakeAffin(scale, rotate, pos);
}

void SkyDome::Draw(const Camera& camera)
{
	rayleighScattering_->cameraPos = camera.GetPos();
	rayleighScattering_->viewDirection = Vector3::kZIdentity * Mat4x4::MakeRotate(camera.rotate);

	if (data_.empty()) {
		data_ = mesh_->CopyBuffer();
	}

	wvpData_->viewProjectoionMat = camera.GetViewProjection();

	auto commandlist = DirectXCommand::GetInstance()->GetCommandList();

	for (auto& i : data_) {
		if (pipeline_) {
			pipeline_->Use();
		}
		tex_->Use(0);

		commandlist->SetGraphicsRootDescriptorTable(1, wvpData_.GetHandleGPU());

		commandlist->IASetVertexBuffers(0, 1, &i.second.view);
		commandlist->DrawInstanced(i.second.vertNum, 1, 0, 0);
	}
}

void SkyDome::SetTexture(Texture* const tex)
{
	if (!tex_) {
		throw Lamb::Error::Code<SkyDome>("tex is nullptr", __func__);
	}
	tex_ = tex;
}

Pipeline* SkyDome::CreatePipeline()
{
	std::array<D3D12_DESCRIPTOR_RANGE, 1> range = {};
	range[0].NumDescriptors = 1;
	range[0].BaseShaderRegister = 0;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	std::array<D3D12_DESCRIPTOR_RANGE, 1> rangeCBV = {};
	rangeCBV[0].NumDescriptors = 2;
	rangeCBV[0].BaseShaderRegister = 0;
	rangeCBV[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rangeCBV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

	std::array<D3D12_ROOT_PARAMETER, 3> paramates = {};
	paramates[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	paramates[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	paramates[0].DescriptorTable.pDescriptorRanges = range.data();
	paramates[0].DescriptorTable.NumDescriptorRanges = UINT(range.size());

	paramates[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	paramates[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	paramates[1].DescriptorTable.pDescriptorRanges = rangeCBV.data();
	paramates[1].DescriptorTable.NumDescriptorRanges = UINT(rangeCBV.size());

	PipelineManager::CreateRootSgnature(paramates.data(), paramates.size(), true);
	
	Shader shader = LoadShader();

	PipelineManager::SetShader(shader);

	PipelineManager::SetVertexInput("POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetVertexInput("NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT);

	PipelineManager::SetState(Pipeline::Blend::Normal, Pipeline::SolidState::Solid);

	Pipeline* pipeline = PipelineManager::Create();

	PipelineManager::StateReset();

	return pipeline;
}

Shader SkyDome::LoadShader()
{
	ShaderManager* const shaderManager = ShaderManager::GetInstance();

	Shader result;

	result.vertex = shaderManager->LoadVertexShader("./Resources/Shaders/SkyDomeShader/SkyDome.VS.hlsl");
	result.pixel = shaderManager->LoadPixelShader("./Resources/Shaders/SkyDomeShader/SkyDome.PS.hlsl");

	return result;
}
