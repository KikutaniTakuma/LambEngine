#include "AnimationModel.h"
#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"

AnimationModel::AnimationModel(const std::string& fileName) :
	AnimationModel()
{
	Load(fileName);
}

void AnimationModel::Load(const std::string& fileName) {
	Model::Load(fileName);
	animator_.reset();
	animator_ = std::make_unique<Animator>();
	animator_->Load(fileName);
}

void AnimationModel::Update() {
	animator_->Update(renderSet->GetMesh());
}

void AnimationModel::Draw(
	const Mat4x4& worldMatrix,
	const Mat4x4& camera,
	uint32_t color,
	BlendType blend,
	bool isLighting
) {
	RenderContext<>* renderContext = renderSet->GetRenderContextDowncast<RenderContext<>>(blend);
	renderContext->SetSahderStruct(static_cast<uint32_t>(isLighting));

	Draw(worldMatrix, camera, color, blend);
}

void AnimationModel::Draw(
	const Mat4x4& worldMatrix,
	const Mat4x4& camera,
	uint32_t color,
	BlendType blend
) {
	RenderData* render = renderSet->GetRenderContext(blend);
	const Mat4x4& animationMatrix = animator_->GetIsActive() ? animator_->GetLocalMat4x4() : renderSet->GetNode().loacalMatrix;

	render->SetWVPMatrix({
		animationMatrix * worldMatrix,
		camera
		}
	);

	render->SetColor(color);

	render->AddDrawCount();
}
