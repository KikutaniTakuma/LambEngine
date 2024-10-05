#include "Texture2D.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#include "Utils/SafePtr.h"
#include "../DrawerManager.h"
#include "Utils/EngineInfo.h"

#include "Engine/Graphics/RenderingManager/RenderingManager.h"

#ifdef USE_DEBUG_CODE
#include "Utils/FileUtils.h"

#include "imgui.h"
#endif // USE_DEBUG_CODE


const LoadFileNames Texture2D::kFileNames_ = 
		LoadFileNames{
			.resourceFileName{"./Resources/EngineResources/Texture2D/Texture2D.obj"},
			.shaderName{
				.vsFileName = "./Shaders/Texture2DShader/Texture2D.VS.hlsl",
				.psFileName = "./Shaders/Texture2DShader/Texture2D.PS.hlsl"
			}
		};

const MeshLoadFileNames Texture2D::kMeshFileNames_ = MeshLoadFileNames{
	.resourceFileName{"./Resources/EngineResources/Texture2D/Texture2D.obj"},
	.shaderName{
		.asFileName = "./Shaders/Texture2DMeshShader/MeshTex2D.AS.hlsl",
		.msFileName = "./Shaders/Texture2DMeshShader/MeshTex2D.MS.hlsl",
		.psFileName = "./Shaders/Texture2DMeshShader/MeshTex2D.PS.hlsl"
	}
};

Texture2D::Texture2D():
	BaseDrawer()
{}

void Texture2D::Load()
{
	Lamb::SafePtr renderContextManager = RenderContextManager::GetInstance();

	// メッシュシェーダーが読み込む
	if (Lamb::IsCanUseMeshShader()) {
		// リソースとメッシュシェーダー読み込み
		renderContextManager->LoadMesh<Texture2D::ShaderData, Texture2D::kMaxDrawCount>(kMeshFileNames_, 4);

		meshRenderSet = renderContextManager->Get(kMeshFileNames_);
	}

	// リソースとシェーダー読み込み
	renderContextManager->Load<Texture2DRenderContext>(kFileNames_, 4);

	renderSet = renderContextManager->Get(kFileNames_);
}

void Texture2D::Draw(
	const Mat4x4& worldMatrix,
	const Mat4x4& uvTransform,
	const Mat4x4& camera,
	uint32_t textureID,
	uint32_t color,
	BlendType blend
) {
#ifdef USE_DEBUG_CODE
	isUseMeshShader_ = RenderingManager::GetInstance()->GetIsUseMeshShader();
#endif // USE_DEBUG_CODE

	if (isUseMeshShader_ and meshRenderSet) {
		Lamb::SafePtr renderContext = meshRenderSet->GetRenderContextDowncast<MeshRenderContext<Texture2D::ShaderData, Texture2D::kMaxDrawCount>>(blend);

		renderContext->SetShaderStruct(
			ShaderData{
				.uvTransform = uvTransform,
				.pad = Vector3::kZero,
				.textureID = textureID
			}
		);
	}
	else {
		Lamb::SafePtr renderContext = renderSet->GetRenderContextDowncast<Texture2DRenderContext>(blend);

		renderContext->SetShaderStruct(
			ShaderData{
				.uvTransform = uvTransform,
				.pad = Vector3::kZero,
				.textureID = textureID
			}
		);
	}

	BaseDrawer::Draw(worldMatrix, camera, color,  blend);
}

void Texture2D::Draw(const Texture2D::Data& data) {
#ifdef USE_DEBUG_CODE
	isUseMeshShader_ = RenderingManager::GetInstance()->GetIsUseMeshShader();
#endif // USE_DEBUG_CODE

	if (isUseMeshShader_ and meshRenderSet) {
		Lamb::SafePtr renderContext = meshRenderSet->GetRenderContextDowncast<MeshRenderContext<Texture2D::ShaderData, Texture2D::kMaxDrawCount>>(data.blend);

		renderContext->SetShaderStruct(
			ShaderData{
				.uvTransform = data.uvTransform,
				.pad = Vector3::kZero,
				.textureID = data.textureID
			}
		);
	}
	else {
		Lamb::SafePtr renderContext = renderSet->GetRenderContextDowncast<Texture2DRenderContext>(data.blend);

		renderContext->SetShaderStruct(
			ShaderData{
				.uvTransform = data.uvTransform,
				.pad = Vector3::kZero,
				.textureID = data.textureID
			}
		);
	}

	BaseDrawer::Draw(data.worldMatrix, data.camera, data.color, data.blend);
}

void Texture2D::AllDraw() {
	renderSet->Draw();
	renderSet->ResetDrawCount();
}

void Tex2DInstance::Load(const std::string& fileName) {
	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadTexture(fileName);
	tex_ = TextureManager::GetInstance()->GetTexture(fileName);
	tex2D_ = drawerManager->GetTexture2D();
}

void Tex2DInstance::Draw(const Mat4x4& cameraMat)
{
	if (tex2D_.have()) {
		tex2D_->Draw(
			Mat4x4::MakeAffin(scale, rotate, pos),
			Mat4x4::MakeAffin(Vector3(uvSize, 1.0f), Vector3::kZero, Vector3(uvPibot, 0.0f)),
			cameraMat,
			tex_->GetHandleUINT(),
			color,
			blend
		);
	}
}

bool Tex2DInstance::Collision(const Vector3& otherPos)
{
	Vector3 min = pos - (scale * 0.5f);
	Vector3 max = pos + (scale * 0.5f);

	return (min.x < otherPos.x and otherPos.x < max.x) and (min.y < otherPos.y and otherPos.y < max.y);
}
