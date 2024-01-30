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
	camera_->pos.y = 6.46f;
	camera_->rotate = { 0.18f, 0.17f, 0.0f };

	water_ = Water::GetInstance();

	uiCamera_.reset(new Camera{});
	uiCamera_->Update();

	player_.reset(new Model{ "./Resources/Player/Player.obj" });
	player_->pos = { 4.38f, 3.22f, 10.590f };
	player_->rotate.y = -0.42f;
	player_->scale *= 2.0f;

	str_.SetFormat("./Resources/Font/mincho_size_32.spritefont");
	str_ << "水面2";
	str_.scale *= 14.8f * 0.5f;
	str_.pos = { 52.0f, 136.0f };
	str_.color = 0xff;

	startMessage_.SetFormat("./Resources/Font/mincho_size_32.spritefont");
	startMessage_ << "Ａボタン押してください";
	startMessage_.pos = { 144.0f, 539.0f };

	cloud_ = Cloud::GetInstance();

	skydome_.reset(new SkyDome);
	skydome_->Initialize();
	skydome_->SetTexture(cloud_->GetTex());

	waterSE_ = audioManager_->LoadWav("./Resources/Sound/SE_Water.wav", true);
	waterSE_->Start(0.5f);

	inGameSE_ = audioManager_->LoadWav("./Resources/Sound/SE_InGame.wav", false);
}

void TitleScene::Finalize()
{
	waterSE_->Stop();
	inGameSE_->Stop();
}

void TitleScene::Update()
{
	cloud_->Update();
	skydome_->Upadate();

	camera_->Debug("camera");
	camera_->Update();

	player_->Debug("player");
	player_->Update();


	water_->Update(camera_->GetPos());

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
		inGameSE_->Start(0.8f);
	}

	messageAlpah_ += std::numbers::pi_v<float> *0.5f * Lamb::DeltaTime();
	startMessage_.color = static_cast<uint32_t>(255.0f * std::abs(std::cos(messageAlpah_)));
}

void TitleScene::Draw()
{
	cloud_->Draw();
	skydome_->Draw(*camera_);

	water_->Draw(camera_->GetViewProjection());
	player_->Draw(camera_->GetViewProjection(), camera_->GetPos());

	str_.Draw();
	startMessage_.Draw();
}
