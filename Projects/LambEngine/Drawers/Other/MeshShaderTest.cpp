#include "MeshShaderTest.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr.h"
#include "../DrawerManager.h"

#ifdef _DEBUG
#include "Utils/FileUtils.h"

#include "imgui.h"
#endif // _DEBUG


const MeshLoadFileNames MeshShaderTest::kFileNames_ = 
		MeshLoadFileNames{
			.resourceFileName{"./Resources/EngineResources/Texture2D/Texture2D.obj"},
			.shaderName{
				.msFileName = "./Resources/Shaders/MeshShader/Simple.MS.hlsl",
				.psFileName = "./Resources/Shaders/MeshShader/Simple.PS.hlsl"
			}
		};

MeshShaderTest::MeshShaderTest():
	BaseDrawer()
{}

void MeshShaderTest::Load()
{
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	renderContextManager->LoadMesh<MeshRenderContext>(kFileNames_, 3);

	renderSet = renderContextManager->Get(kFileNames_);
}

void MeshShaderTest::Draw(const Mat4x4& worldMatrix, const Mat4x4& camera, BlendType blend)
{
	BaseDrawer::Draw(worldMatrix, camera, 0, blend);
}

void MeshShaderTest::AllDraw() {
	renderSet->Draw();
	renderSet->ResetDrawCount();
}
