#include "StartArea.h"

#include <numbers>

#include "imgui.h"

#include "Camera/Camera.h"	
#include "Game/StageManager/StageManager.h"
#include "Drawers/DrawerManager.h"

void StartArea::Initialize() {
	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("./Resources/DebugArrow/DebugGoal.obj");
	drawerManager->LoadModel("./Resources/InGame/Models/beach.obj");

	start_ = drawerManager->GetModel("./Resources/DebugArrow/DebugGoal.obj");
	leftBeach_ = drawerManager->GetModel("./Resources/InGame/Models/beach.obj");
	rightBeach_ = drawerManager->GetModel("./Resources/InGame/Models/beach.obj");

	startInstance_ = std::make_unique<Model::Instance>();
	leftBeachInstance_ = std::make_unique<Model::Instance>();
	rightBeachInstance_ = std::make_unique<Model::Instance>();
	startInstance_->transform.scale = { 10.0f ,10.0f ,10.0f };
	startInstance_->transform.rotate = Quaternion::MakeRotateXAxis(180.0f * Lamb::Math::toRadian<float>);
	startInstance_->transform.translate;
	leftBeachInstance_->transform.translate.x += startInstance_->transform.scale.x * 0.5f;
	rightBeachInstance_->transform.translate.x -= startInstance_->transform.scale.x * 0.5f;
	leftBeachInstance_->transform.SetParent(&startInstance_->transform);
	rightBeachInstance_->transform.SetParent(&startInstance_->transform);
}

void StartArea::Update() {
#ifdef _DEBUG
	ImGui::Begin("Start");
	ImGui::DragFloat3("scale", startInstance_->transform.scale.data(), 0.1f);
	ImGui::DragFloat4("rotate", startInstance_->transform.rotate.data(), 0.1f);
	ImGui::DragFloat3("pos", startInstance_->transform.translate.data(), 0.1f);
	ImGui::End();
#endif // _DEBUG

	ResourceUpdate();
}

void StartArea::ResourceUpdate() {
	startInstance_->transform.CalcMatrix();
	leftBeachInstance_->transform.CalcMatrix();
	rightBeachInstance_->transform.CalcMatrix();
}

void StartArea::Draw(const Camera& camera) {
	if (StageManager::GetStage() != 9) {
		start_->Draw(
			startInstance_->transform.GetMatrix(),
			camera.GetViewProjection(),
			startInstance_->color,
			BlendType::kNormal
		);
		leftBeach_->Draw(
			leftBeachInstance_->transform.GetMatrix(),
			camera.GetViewProjection(),
			leftBeachInstance_->color,
			BlendType::kNormal
		);
		rightBeach_->Draw(
			rightBeachInstance_->transform.GetMatrix(),
			camera.GetViewProjection(),
			rightBeachInstance_->color,
			BlendType::kNormal
		);
	}
}
