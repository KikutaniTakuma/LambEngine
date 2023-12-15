#include "PipelineObject.h"
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include <cassert>

void PipelineObject::LoadShader(const std::string& vsShader, const std::string& psShader) {
	static ShaderManager* const shaderManager = ShaderManager::GetInstance();
	shader_.vertex = shaderManager->LoadVertexShader(vsShader);
	assert(shader_.vertex);
	shader_.pixel = shaderManager->LoadPixelShader(psShader);
	assert(shader_.pixel);
}