#include "SkyBlock.h"
#include "Camera/Camera.h"

void SkyBlock::Init(const Transform& transform) {
	transform_ = transform;
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
