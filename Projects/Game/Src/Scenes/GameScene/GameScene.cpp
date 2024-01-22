#include "GameScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
#include "Utils/ScreenOut/ScreenOut.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	tex_.reset(new Texture2D{});
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	
	tex_->Debug("tex");
	tex_->Update();

	if (input_->GetKey()->Pushed(DIK_SPACE)) {
		sceneManager_->SceneChange(BaseScene::ID::Title);
	}
}

void GameScene::Draw() {
	camera_->Update();
	tex_->Draw(camera_->GetViewOthographics());

	Lamb::screenout << "Game Scnene" << Lamb::endline;
}