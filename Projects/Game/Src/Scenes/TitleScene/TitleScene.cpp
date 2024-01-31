#include "TitleScene.h"
#include "Game/Water/Water.h"
#include <cmath>
#include <numbers>
#include "Utils/EngineInfo/EngineInfo.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"

TitleScene::TitleScene():
	BaseScene{BaseScene::ID::Title}
{
}

void TitleScene::Initialize()
{
	camera_->pos.z = -10.0f;

	model_.reset(new Model{ "./Resources/Hololive/Watame/Watame.obj" });
}

void TitleScene::Finalize()
{
	
}

void TitleScene::Update()
{
	camera_->Debug("camera");
	camera_->Update();

	model_->Debug("player");
	model_->Update();

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}
}

void TitleScene::Draw()
{
	model_->Draw(camera_->GetViewProjection(), camera_->GetPos());
}
