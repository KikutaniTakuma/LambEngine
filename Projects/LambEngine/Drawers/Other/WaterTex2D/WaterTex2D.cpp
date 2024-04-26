#include "WaterTex2D.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr/SafePtr.h"

#ifdef _DEBUG
#include "Utils/UtilsLib/UtilsLib.h"

#include "imgui.h"
#endif // _DEBUG

uint32_t WaterTex2D::kCausticsTextureID_ = 0u;

const LoadFileNames WaterTex2D::kFileNames_ =
LoadFileNames{
	.resourceFileName{"./Resources/EngineResources/Texture2D/Texture2D.obj"},
	.shaderName{
		.vsFileName = "./Resources/Shaders/WaterTex2DShader/WaterTex2D.VS.hlsl",
		.psFileName = "./Resources/Shaders/WaterTex2DShader/WaterTex2D.PS.hlsl"
	}
};

WaterTex2D::WaterTex2D() :
	BaseDrawer()
{}

void WaterTex2D::Load()
{
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	renderContextManager->Load<WaterRenderContext>(kFileNames_);

	renderSet = renderContextManager->Get(kFileNames_);

	kCausticsTextureID_ = TextureManager::GetInstance()->LoadTexture("./Resources/Common/Water/caustics_01.bmp");
}

void WaterTex2D::Draw(
	const Mat4x4& worldMatrix,
	const Mat4x4& camera,
	Vector2 randomVec,
	uint32_t color,
	BlendType blend
) {
	Lamb::SafePtr renderContext = renderSet->GetRenderContextDowncast<WaterRenderContext>(blend);

	renderContext->SetSahderStruct(
		ShaderData{
			.randomVec = randomVec,
			.normal  = Vector3(0.0f,0.0f,1.0f),
			.pad = 0.0f,
			.tangent = Vector3(0.0f,-1.0f,0.0f),
			.textureID = kCausticsTextureID_
		}
	);

	BaseDrawer::Draw(worldMatrix, camera, color, blend);
}
