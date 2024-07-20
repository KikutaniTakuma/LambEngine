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
	levelData_ = LevelLoader::Load("./SceneData/TitleScene.json");
	ObjectManager::GetInstance()->SetLevelData(levelData_, currentCamera_);
}

void TitleScene::Initialize()
{
	currentCamera_->pos = { -0.216f, 6.692f, -1.256f };
	currentCamera_->rotate = { 0.18f, 0.17f, 0.0f };

	water_ = Water::GetInstance();

	uiCamera_.reset(new Camera{});
	uiCamera_->Update();

	//player_ = drawerManager_->GetModel("./Resources/Player/Player.obj");
	//playerTransform_.translate = { 4.38f, 3.22f, 10.590f };
	//playerTransform_.rotate.y = -0.42f;
	//playerTransform_.scale *= 2.0f;

	str_.SetFormat("./Resources/Font/mincho_size_32.spritefont");
	str_ << "Falling";
	str_.scale *= 14.8f * 0.5f;
	str_.pos = { 52.0f, 136.0f };
	str_.color = 0xff;

	startMessage_.SetFormat("./Resources/Font/mincho_size_32.spritefont");
	startMessage_ << "Ａボタン押してください";
	startMessage_.pos = { 144.0f, 539.0f };

	/*cloud_ = Cloud::GetInstance();

	skydome_.reset(new SkyDome);
	skydome_->Initialize();
	skydome_->SetTexture(cloud_->GetTex());*/

	/*waterSE_ = audioManager_->Get("./Resources/Sound/SE_Water.wav");
	waterSE_->Start(0.5f, true);

	inGameSE_ = audioManager_->Get("./Resources/Sound/SE_InGame.wav");*/

}

void TitleScene::Finalize()
{
	ObjectManager::GetInstance()->Clear();
	levelData_.reset();
}

void TitleScene::Update()
{
	/*cloud_->Update();
	skydome_->Upadate();*/

	//currentCamera_->Debug("camera");
	currentCamera_->Update();

	water_->Debug("water");
	water_->Update(currentCamera_->GetPos());

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
		//inGameSE_->Start(0.8f, false);
	}

	messageAlpah_ += std::numbers::pi_v<float> *0.5f * Lamb::DeltaTime();
	startMessage_.color = static_cast<uint32_t>(255.0f * std::abs(std::cos(messageAlpah_)));

	ObjectManager::GetInstance()->Update();
}

void TitleScene::Draw()
{
	water_->Draw(ObjectManager::GetInstance()->GetCameraMatrix());
	/*player_->Draw(
		playerTransform_.GetMatrix(),
		currentCamera_->GetViewProjection(),
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNormal
	);*/

	ObjectManager::GetInstance()->Draw();

	//sceneManager_->AllDraw();

	/*str_.Draw();
	startMessage_.Draw();*/

}