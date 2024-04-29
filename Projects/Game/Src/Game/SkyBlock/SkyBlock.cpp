#include "SkyBlock.h"
#include "Camera/Camera.h"

#include "Drawers/DrawerManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void SkyBlock::Init(const Transform& transform) {
	transform_ = transform;

	Lamb::SafePtr draweManager = DrawerManager::GetInstance();
	draweManager->LoadModel("./Resources/Common/Cube.obj");
	model_ = draweManager->GetModel("./Resources/Common/Cube.obj");

	isFall_ = false;
	gravity_ = 9.80665f;
	speed_ = 0.0f;
	fallingTime_ = 0.0f;
}

void SkyBlock::Update() {
	if (isFall_) {
		Falling();
	}

	transform_.translate.y += speed_;
}

void SkyBlock::Draw(const Camera& camera) {
	model_->Draw(
		transform_.GetMatrix(),
		camera.GetViewProjection(),
		0xffffffff,
		BlendType::kNormal
	);
}

void SkyBlock::StartFall() {
	isFall_ = true;
}

void SkyBlock::Debug([[maybe_unused]]const std::string& guiName)
{
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	transform_.Debug(guiName.c_str());
	if (ImGui::TreeNode("skyBlock state")) {
		ImGui::Checkbox("isFalling", isFall_.data());


		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG
}

void SkyBlock::Falling() {
	if (transform_.translate.y < 0.0f) {
		transform_.translate.y = 0.0f;
		speed_ = 0.0f;
		fallingTime_ = 0.0f;
		isFall_ = false;
	}
	else {
		speed_ = -gravity_ * fallingTime_;
		fallingTime_ += Lamb::DeltaTime();
	}
}
