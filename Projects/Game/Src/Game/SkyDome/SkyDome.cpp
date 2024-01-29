#include "SkyDome.h"
#include "Utils/Camera/Camera.h"

void SkyDome::Initialize()
{
	model_.reset(new Model{ "./Resources/skydome/skydome.obj" });
	model_->SetIsLighting(false);
	scale = Vector3::kIdentity * 300.0f;
}

void SkyDome::Upadate()
{
	model_->pos = pos;
	model_->rotate = rotate;
	model_->scale = scale;
	model_->Update();
}

void SkyDome::Draw(const Camera& camera)
{
	model_->Draw(camera.GetViewProjection(), camera.pos);
}

void SkyDome::SetTexture(Texture* const tex)
{
	model_->ChangeTexture("None", tex);
}
