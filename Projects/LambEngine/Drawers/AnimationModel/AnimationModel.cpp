#include "AnimationModel.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"

AnimationModel::AnimationModel(const std::string& fileName) :
	AnimationModel()
{
	Load(fileName);
}

void AnimationModel::Load(const std::string& fileName)
{
	Model::Load(fileName);
	animator.Load(fileName);
}

void AnimationModel::Update()
{
	animator.Update(renderSet->GetMesh());
}

void AnimationModel::Draw(const Mat4x4& camera, BlendType blend, bool isLighting)
{
	RenderContext<>* renderContext = renderSet->GetRenderContextDowncast<RenderContext<>>(blend);
	renderContext->SetSahderStruct(static_cast<uint32_t>(isLighting));

	Draw(camera, blend);
}

void AnimationModel::Draw(const Mat4x4& camera, BlendType blend)
{
	RenderData* render = renderSet->GetRenderContext(blend);

	render->SetWVPMatrix({
		animator.GetLocalMat4x4() * Mat4x4::MakeAffin(scale, rotate, translate),
		camera
		}
	);

	render->SetColor(color);

	render->AddDrawCount();
}
