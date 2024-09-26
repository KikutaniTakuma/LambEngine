#include "MeshShaderTest.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr.h"
#include "../DrawerManager.h"

#ifdef _DEBUG
#include "Utils/FileUtils.h"

#include "imgui.h"
#endif // _DEBUG


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
				.msFileName = "./Resources/Shaders/MeshShader/Simple.MS.hlsl",
				.psFileName = "./Resources/Shaders/MeshShader/Simple.PS.hlsl"
			}
	};

	renderContextManager->LoadMesh<MeshRenderContext>(fileNames, 3);

	renderSet = renderContextManager->Get(fileNames);
}

void MeshShaderTest::Draw(const Mat4x4& worldMatrix, const Mat4x4& camera, BlendType blend)
{
	BaseDrawer::Draw(worldMatrix, camera, 0, blend);
}

void MeshShaderTest::AllDraw() {
	renderSet->Draw();
	renderSet->ResetDrawCount();
}
