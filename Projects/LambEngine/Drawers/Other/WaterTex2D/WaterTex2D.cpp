#include "WaterTex2D.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr.h"

#ifdef _DEBUG
#include "Utils/FileUtils.h"

#include "imgui.h"
#endif // _DEBUG

uint32_t WaterTex2D::kCausticsTextureID_ = 0u;

const LoadFileNames WaterTex2D::kFileNames_ =
LoadFileNames{
	.resourceFileName{"./Resources/EngineResources/Texture2D/Texture2D.obj"},
	.shaderName = ShaderFileNames{
		.vsFileName = "./Resources/Shaders/WaterTex2DShader/WaterTex2D.VS.hlsl",
		.psFileName = "./Resources/Shaders/WaterTex2DShader/WaterTex2D.PS.hlsl",
		.gsFileName = "./Resources/Shaders/WaterTex2DShader/WaterTex2D.GS.hlsl",
		.dsFileName = "./Resources/Shaders/WaterTex2DShader/WaterTex2D.DS.hlsl",
		.hsFileName = "./Resources/Shaders/WaterTex2DShader/WaterTex2D.HS.hlsl",
	}
};

WaterTex2D::WaterTex2D() :
	BaseDrawer()
{}

void WaterTex2D::Load()
{
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	renderContextManager->Load<WaterRenderContext>(kFileNames_, 3);

	renderSet = renderContextManager->Get(kFileNames_);

	Lamb::SafePtr textureManager = TextureManager::GetInstance();
	textureManager->LoadTexture("./Resources/Common/Water/caustics_01.bmp");

	kCausticsTextureID_ = textureManager->GetHandle("./Resources/Common/Water/caustics_01.bmp");
}

void WaterTex2D::Draw(
	const Mat4x4& worldMatrix,
	const Mat4x4& camera,
	Vector2 randomVec,
	float32_t density,
	uint32_t edgeDivision,
	uint32_t insideDivision,
	WaveData waveData,
	uint32_t color,
	BlendType blend
) {
	Lamb::SafePtr renderContext = renderSet->GetRenderContextDowncast<WaterRenderContext>(blend);

	renderContext->SetShaderStruct(
		ShaderData{
			.randomVec = randomVec,
			.normal  = Vector3(0.0f,1.0f,0.0f),
			.tangent = Vector3(0.0f,0.0f,1.0f),
			.textureID = kCausticsTextureID_,
			.density = density,
			.edgeDivision = std::clamp(edgeDivision, 1u, 64u),
			.insideDivision = std::clamp(insideDivision, 1u, 64u),
			.waveData = waveData
		}
	);

	BaseDrawer::Draw(worldMatrix, camera, color, blend);
}

void WaterTex2D::AllDraw(BlendType blend)
{
	Lamb::SafePtr renderContext = renderSet->GetRenderContext(blend);
	renderContext->Draw();
	renderContext->ResetDrawCount();
}
