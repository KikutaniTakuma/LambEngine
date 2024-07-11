#include "GameScene.h"
#include "Game/Water/Water.h"
#include <cmath>
#include <numbers>
#include "Utils/EngineInfo.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut.h"

#include "Utils/Random.h"
#include "imgui.h"
#include "Game/Water/Water.h"

GameScene::GameScene() :
	BaseScene{ BaseScene::ID::Title }
{
}

void GameScene::Load()
{
	levelData_ = LevelLoader::Load("./SceneData/GameScene.json");
}

void GameScene::Initialize()
{
	//currentCamera_->pos.y = 2.85f;
	currentCamera_->offset.y = -0.7f;
	currentCamera_->offset.z = -55.0f;
	currentCamera_->rotate.x = 0.37f;


	currentCamera_->Update();

	water_ = Water::GetInstance();

	coin_ = std::make_unique<Coin>();
	Transform coinTransform;
	coinTransform.scale *= 2.0f;
	coin_->Init(coinTransform);

}

void GameScene::Finalize()
{
	levelData_.reset();
}

void GameScene::Update()
{
	currentCamera_->Debug("カメラ");

	levelData_->player->Update(currentCamera_->rotate);

	coin_->Update();

	for (auto& i : levelData_->skyBlocks) {
		i->Update();
	}


	/*if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}*/

	Vector3 pushVector;
	bool isCollision = false;

	for (auto& i : levelData_->skyBlocks) {
		isCollision = i->GetObb().IsCollision(levelData_->player->GetObb(), pushVector);

		if (isCollision) {
			levelData_->player->AfterCollisionUpdate(pushVector);
			if (levelData_->player->GetIsPunch().OnExit()) {
				i->StartFall();
			}
			break;
		}
	}


	levelData_->player->Landing(isCollision);

	coin_->SetIsCollision(coin_->GetObb().IsCollision(levelData_->player->GetObb()));


#ifdef _DEBUG
	if (not isDebug_) {
		currentCamera_->Update(levelData_->player->GetTranslate());
	}
	else {
		currentCamera_->Update();
	}
#else
	currentCamera_->Update(levelData_->player->GetTranslate());
#endif // DEBUG


	water_->Update(currentCamera_->GetPos());
	postEffectManager_->GetPera().Debug("pera");
	postEffectManager_->GetPera().Update();
}

void GameScene::Draw()
{
	water_->Draw(currentCamera_->GetViewProjection());

	for (auto& i : levelData_->skyBlocks) {
		i->Draw(*currentCamera_);
	}

	levelData_->player->Draw(*currentCamera_);

	coin_->Draw(*currentCamera_);

	Lamb::screenout << "Model scene" << Lamb::endline
		<< "Press space to change ""Water and cloud scene""";
}
