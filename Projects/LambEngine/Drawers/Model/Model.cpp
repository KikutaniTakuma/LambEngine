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
		RenderContextManager::LoadFileNames{
			.reourceFileName = fileName,
			.shaderName{
				.vsFileName = "./Resources/Shaders/ModelShader/Model.VS.hlsl",
				.psFileName = "./Resources/Shaders/ModelShader/Model.PS.hlsl",
			}
		}
	);
}