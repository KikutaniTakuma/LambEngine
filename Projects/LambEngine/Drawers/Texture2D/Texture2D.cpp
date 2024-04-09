#include "Texture2D.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"

#ifdef _DEBUG
#include "Utils/UtilsLib/UtilsLib.h"

#include "imgui.h"
#endif // _DEBUG


const LoadFileNames Texture2D::kFileNames_ = 
		LoadFileNames{
			.resourceFileName{"./Resources/Texture2D/Texture2D.obj"},
			.shaderName{
				.vsFileName = "./Resources/Shaders/Texture2DShader/Texture2D.VS.hlsl",
				.psFileName = "./Resources/Shaders/Texture2DShader/Texture2D.PS.hlsl"
			}
		};

Texture2D::Texture2D():
	BaseDrawer(),
	uvScale(Vector3::kIdentity),
	uvRotate(),
	uvTranslation(),
	textureID_(0)
{

}

Texture2D::Texture2D(const std::string& fileName):
	Texture2D()
{
	Load(fileName);
}

void Texture2D::Load(const std::string& fileName)
{
	textureID_ = TextureManager::GetInstance()->LoadTexture(fileName);

	RenderContextManager* const renderContext = RenderContextManager::GetInstance();

	renderContext->Load<Texture2DRenderContext>(kFileNames_);

	renderSet = renderContext->Get(kFileNames_);
}

void Texture2D::Draw(const Mat4x4& camera, BlendType blend) {
	Texture2DRenderContext* renderContext = renderSet->GetRenderContextDowncast<Texture2DRenderContext>(blend);

	renderContext->SetSahderStruct(
		ShaderData{
			.uvTransform = Mat4x4::MakeAffin(
				uvScale,
				uvRotate,
				uvTranslation
			),
			.pad = Vector3::kZero,
			.textureID = textureID_
		}
	);

	BaseDrawer::Draw(camera, blend);
}

void Texture2D::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	BaseDrawer::Debug(guiName);

	ImGui::Begin(guiName.c_str());
	if (ImGui::TreeNode("uv transform")) {
		ImGui::DragFloat2("大きさ", &uvScale.x,0.01f);
		ImGui::DragFloat("回転", &uvRotate.z, 0.01f);
		ImGui::DragFloat2("ポジション", &uvTranslation.x, 0.01f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("texture")) {
		const auto& fileNames = Lamb::GetFilePathFormDir("./Resources/", ".png");

		for (const auto& i : fileNames) {
			if (ImGui::Button(("Load " + i.string()).c_str())) {
				textureID_ = TextureManager::GetInstance()->LoadTexture(i.string());
				break;
			}

		}

		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG
}
