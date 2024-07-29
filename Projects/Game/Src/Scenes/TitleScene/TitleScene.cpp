#include "TitleScene.h"
#include "Game/Water/Water.h"
#include <cmath>
#include <numbers>
#include "Utils/EngineInfo.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/Random.h"

#include "Engine/Graphics/PipelineObject/Outline/Outline.h"

#include "GameObject/Manager/ObjectManager.h"
#include "Level/LevelLoader.h"

TitleScene::TitleScene() :
	BaseScene{ BaseScene::ID::Title }
{
}

void TitleScene::Load()
{
	ObjectManager::GetInstance()->Load("./SceneData/TitleScene.json");
}

void TitleScene::Initialize()
{
	currentCamera_->pos = { -0.216f, 6.692f, -1.256f };
	currentCamera_->rotate = { 0.18f, 0.17f, 0.0f };

	water_ = Water::GetInstance();

}

void TitleScene::Finalize()
{
	ObjectManager::GetInstance()->Clear();
}

void TitleScene::Update()
{
	currentCamera_->Update();

	water_->Debug("water");
	water_->Update(currentCamera_->GetPos());

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}

	ObjectManager::GetInstance()->Update();
}

void TitleScene::Draw()
{
	water_->Draw(ObjectManager::GetInstance()->GetCameraMatrix());

	ObjectManager::GetInstance()->Draw();
}