#include "SkyBlock.h"
#include "Camera/Camera.h"

#include "Drawers/DrawerManager.h"

void SkyBlock::Init(const Transform& transform) {
	transform_ = transform;

	Lamb::SafePtr draweManager = DrawerManager::GetInstance();
	draweManager->LoadModel("./Resources/Common/Cube.obj");
	model_ = draweManager->GetModel("./Resources/Common/Cube.obj");
}

void SkyBlock::Update() {

}

void SkyBlock::Draw(const Camera& camera) {
	model_->Draw(
		transform_.GetMatrix(),
		camera.GetViewProjection(),
		0xffffffff,
		BlendType::kNormal
	);
}
