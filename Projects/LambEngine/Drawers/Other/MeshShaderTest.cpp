#include "MeshShaderTest.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr.h"
#include "../DrawerManager.h"

#ifdef USE_DEBUG_CODE
#include "Utils/FileUtils.h"

#include "imgui.h"
#endif // USE_DEBUG_CODE


MeshShaderTest::MeshShaderTest():
	BaseDrawer()
{}

void MeshShaderTest::Load(const std::string& fileName)
{
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	MeshLoadFileNames fileNames =
		MeshLoadFileNames{
			.resourceFileName = fileName,
			.shaderName{
				.asFileName = "./Shaders/MeshShader/MeshModel.AS.hlsl",
				.msFileName = "./Shaders/MeshShader/MeshModel.MS.hlsl",
				.psFileName = "./Shaders/MeshShader/MeshModel.PS.hlsl"
			}
	};

	renderContextManager->LoadMesh(fileNames, 4);

	renderSet = renderContextManager->Get(fileNames);
}

void MeshShaderTest::Draw(
	const Mat4x4& worldMatrix, 
	const Mat4x4& camera,
	uint32_t color,
	bool isLighting,
	BlendType blend
) {
	if (renderSet) {
		Lamb::SafePtr renderContext = renderSet->GetRenderContextDowncast<MeshRenderContext<>>(blend);
		if (blend == BlendType::kNone) {
			renderContext->SetShaderStruct(static_cast<uint32_t>(false));
		}
		else {
			renderContext->SetShaderStruct(static_cast<uint32_t>(isLighting));
		}

		BaseDrawer::Draw(worldMatrix, camera, color, blend);
	}
}

void MeshShaderTest::AllDraw() {
	renderSet->Draw();
	renderSet->ResetDrawCount();
}
