#include "GameScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;

	tex_.isSameTexSize = true;
	tex_.texScalar = 150.0f;
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");
	tex_.Debug("tex_");
	tex_.Update();

	model_.Debug("model_");
	model_.Update();
}

void GameScene::Draw() {
	camera_->Update(Vector3::kZero);
	
	tex_.Draw(camera_->GetViewOthographics(), Pipeline::Normal,true, false);

	model_.Draw(camera_->GetViewProjection(), camera_->GetPos());
}