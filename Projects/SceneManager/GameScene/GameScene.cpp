#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include "Utils/Math/Quaternion.h"
#include "Game/Command/ICommand/ICommand.h"
#include <numbers>
#include <format>

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game),
	player_{},
	inputHandler_{},
	command_{nullptr}
{}

void GameScene::Initialize() {
	camera_.farClip = 3000.0f;
	camera_.pos.z = -0.1f;

	player_ = std::make_unique<Player>();
	inputHandler_ = std::make_unique<InputHandler>();

	inputHandler_->AssignMoveLeftCommandToPressKeyA();
	inputHandler_->AssignMoveRightCommandToPressKeyD();
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	command_ = inputHandler_->HandleInput();

	player_->Debug();

	player_->ResetMoveVec();

	if (command_) {
		command_->Exec(*player_);
	}

	player_->Update();
}

void GameScene::Draw() {
	camera_.Update();
	
	player_->Draw(camera_.GetViewOthographics());
}