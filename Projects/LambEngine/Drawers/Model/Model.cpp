#include "Model.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"

Model::Model(const std::string& fileName):
	Model()
{
	Load(fileName);
}

void Model::Load(const std::string& fileName) {
	RenderContextManager* const renderContextManager = RenderContextManager::GetInstance();

	renderContextManager->Load(
		LoadFileNames{
			.reourceFileName = fileName,
			.shaderName{
				.vsFileName = "./Resources/Shaders/ModelShader/Model.VS.hlsl",
				.psFileName = "./Resources/Shaders/ModelShader/Model.PS.hlsl",
			}
		}
	);

	renderSet = renderContextManager->Get(
		LoadFileNames{
			.reourceFileName = fileName,
			.shaderName{
				.vsFileName = "./Resources/Shaders/ModelShader/Model.VS.hlsl",
				.psFileName = "./Resources/Shaders/ModelShader/Model.PS.hlsl",
			}
		}
	);

	for (auto& i : *renderSet) {
		i->SetLight(
			Light{
				.ligDirection{-Vector3::kYIndentity},
				.pad0{},
				.ligColor{ Vector3::kIdentity },
			}
		);
	}
}

void Model::Draw(const Camera* camera, BlendType blend, bool isLighting) {
	RenderContext<>* renderContext = renderSet->GetRenderContextDowncast<RenderContext<>>(blend);
	renderContext->SetSahderStruct(static_cast<int32_t>(isLighting));

	BaseDrawer::Draw(camera, blend);
}