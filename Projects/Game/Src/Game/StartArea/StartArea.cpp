#include "StartArea.h"

#include <numbers>

#include "imgui.h"

#include "Camera/Camera.h"	
#include "Game/StageManager/StageManager.h"

void StartArea::Initialize() {
	start_ = std::make_unique<ModelInstance>();
	start_->Load("./Resources/DebugArrow/DebugGoal.obj");
	leftBeach_ = std::make_unique<ModelInstance>();
	leftBeach_->Load("./Resources/InGame/Models/beach.obj");
	rightBeach_ = std::make_unique<ModelInstance>();
	rightBeach_->Load("./Resources/InGame/Models/beach.obj");
	start_->scale = { 10.0f ,10.0f ,10.0f };
	start_->rotate.y = 180.0f * std::numbers::pi_v<float> / 180.0f;
	start_->pos;
	leftBeach_->pos.x += start_->scale.x * 0.5f;
	rightBeach_->pos.x -= start_->scale.x * 0.5f;
	leftBeach_->SetParent(start_.get());
	rightBeach_->SetParent(start_.get());
}

void StartArea::Update() {
#ifdef USE_DEBUG_CODE
	ImGui::Begin("Start");
	ImGui::DragFloat3("scale", &start_->scale.x, 0.1f);
	ImGui::DragFloat3("rotate", &start_->rotate.x, 0.1f);
	ImGui::DragFloat3("pos", &start_->pos.x, 0.1f);
	ImGui::End();
#endif // USE_DEBUG_CODE

	ResourceUpdate();
}

void StartArea::ResourceUpdate() {
	start_->Update();
	leftBeach_->Update();
	rightBeach_->Update();
}

void StartArea::Draw(const Camera& camera) {
	if (StageManager::GetStage() != 9) {
		start_->Draw(camera.GetViewProjection());
		leftBeach_->Draw(camera.GetViewProjection());
		rightBeach_->Draw(camera.GetViewProjection());
	}
}
