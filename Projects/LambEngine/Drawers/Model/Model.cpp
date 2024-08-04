#include "Model.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr.h"
#include "../DrawerManager.h"

Model::Model(const std::string& fileName):
	Model()
{
	Load(fileName);
}

void Model::Load(const std::string& fileName) {
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	renderContextManager->Load(
		LoadFileNames{
			.resourceFileName = fileName,
			.shaderName{
				.vsFileName = "./Resources/Shaders/ModelShader/Model.VS.hlsl",
				.psFileName = "./Resources/Shaders/ModelShader/Model.PS.hlsl",
			}
		},
		3
	);

	renderSet = renderContextManager->Get(
		LoadFileNames{
			.resourceFileName = fileName,
			.shaderName{
				.vsFileName = "./Resources/Shaders/ModelShader/Model.VS.hlsl",
				.psFileName = "./Resources/Shaders/ModelShader/Model.PS.hlsl",
			}
		}
	);
}

void Model::Draw(
	const Mat4x4& worldMatrix,
	const Mat4x4& camera, 
	uint32_t color, 
	BlendType blend, 
	bool isLighting
) {
	Lamb::SafePtr renderContext = renderSet->GetRenderContextDowncast<RenderContext<>>(blend);
	if (blend == BlendType::kNone) {
		renderContext->SetShaderStruct(static_cast<uint32_t>(false));
	}
	else {
		renderContext->SetShaderStruct(static_cast<uint32_t>(isLighting));
	}

	BaseDrawer::Draw(worldMatrix, camera, color, blend);
}

void Model::Draw(const Data& data) {
	Lamb::SafePtr renderContext = renderSet->GetRenderContextDowncast<RenderContext<>>(data.blend);
	if (data.blend == BlendType::kNone) {
		renderContext->SetShaderStruct(static_cast<uint32_t>(false));
	}
	else {
		renderContext->SetShaderStruct(static_cast<uint32_t>(data.isLighting));
	}

	BaseDrawer::Draw(data.worldMatrix, data.camera, data.color, data.blend);
}

const Node& Model::GetNode() const
{
	return renderSet->GetNode();
}

const ModelData& Model::GetModelData() const {
	return *renderSet->GetModelData();
}

void ModelInstance::Load(const std::string& fileName) {
	DrawerManager::GetInstance()->LoadModel(fileName);
	model_ = DrawerManager::GetInstance()->GetModel(fileName);
}

void ModelInstance::Update()
{
	worldMat_ = Mat4x4::MakeAffin(scale, rotate, pos);
	if (parent_.have()) {
		worldMat_ *= parent_->worldMat_;
	}
}

void ModelInstance::Draw(const Mat4x4& cameraMat)
{
	if (model_.have()) {
		model_->Draw(
			worldMat_,
			cameraMat,
			color,
			blend,
			isLighting
		);
	}
}

void ModelInstance::SetParent(ModelInstance* parent)
{
	parent_ = parent;
}

const Mat4x4& ModelInstance::GetWorldMatrix() const
{
	return worldMat_;
}
