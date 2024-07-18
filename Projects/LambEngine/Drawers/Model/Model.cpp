#include "Model.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr.h"

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

	SetLight(
		Light{
			.ligDirection{-Vector3::kYIdentity},
			.pad0{},
			.ligColor{ Vector3::kIdentity },
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

const Node& Model::GetNode() const
{
	return renderSet->GetNode();
}

const ModelData& Model::GetModelData() const {
	return *renderSet->GetModelData();
}
