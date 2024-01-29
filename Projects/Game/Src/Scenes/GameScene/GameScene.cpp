#include "GameScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
#include "Engine/Graphics/PipelineObject/WaterPipeline/WaterPipeline.h"
#include "Game/Water/Water.h"
#include "Game/Player/Player.h"
#include "Game/Enemy/Enemy.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	camera_->rotate.x = 0.04f;
	camera_->offset.z = -60.0f;
	camera_->offset.y = 8.0f;

	water_ = Water::GetInstance();

	player_.reset(new Player{});
	player_->Initialize();

	enemy_.reset(new Enemy{});
	enemy_->Initialize();

	camera_->Update(player_->GetPos());

	uiCamera_.reset(new Camera{});
	uiCamera_->Update();

	startMessage_.reset(new Texture2D{ "./Resources/Message/StartMessage.png" });
	startMessage_->isSameTexSize = true;
	startMessage_->texScalar = 0.63f;

	messageAlpah_ = 1.0f;
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");

	if (player_->IsGameOver()) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}
	//water_->Debug("water");


	player_->Move();
	//player_->Debug("player");
	player_->Update(*camera_);


	enemy_->Debug("Boss");
	enemy_->Update(*player_, *camera_);


	player_->Attack(*enemy_);

	player_->Collision(*enemy_);
	enemy_->Collision(*player_);


	camera_->rotate.y = player_->GetRotate();
	camera_->Update(player_->GetPos());
	water_->Update(camera_->GetPos());

	//startMessage_->Debug("startMessage_");
	if (0.0f < messageAlpah_) {
		messageAlpah_ -= 0.2f * Lamb::DeltaTime();
	}
	else {
		messageAlpah_ = 0.0f;
	}
	startMessage_->color = Vector4ToUint({1.0f,1.0f,1.0f,std::max(messageAlpah_,0.0f )});
	startMessage_->Update();

	if (messageAlpah_ == 0.0f) {
		enemy_->StartAttack();
	}

	if (input_->GetKey()->Pushed(DIK_ESCAPE) || input_->GetGamepad()->Pushed(Gamepad::Button::START)) {
		sceneManager_->SceneChange(BaseScene::ID::Title);
	}

	if (enemy_->IsGameClear()) {
		sceneManager_->SceneChange(BaseScene::ID::Title);
	}

}

void GameScene::Draw() {
	meshManager_->ResetDrawCount();


	water_->Draw(camera_->GetViewProjection());

	player_->Draw(*camera_);

	enemy_->Draw(*camera_);

	meshManager_->Draw();

	player_->AfterDraw();

	enemy_->AfterDraw();

	startMessage_->Draw(uiCamera_->GetViewOthographics());
}