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
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"

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


	cloud_ = Cloud::GetInstance();

	skydome_.reset(new SkyDome);
	skydome_->Initialize();
	skydome_->SetTexture(cloud_->GetTex());

	waterSE_ = audioManager_->Load("./Resources/Sound/SE_Water.wav");
	waterSE_->Start(0.5f, true);

	clearSE_ = audioManager_->Load("./Resources/Sound/SE_Clear.wav");
	playerDamageSE_ = audioManager_->Load("./Resources/Sound/SE_Player_Damage.wav");
	enemyDamageSE_ = audioManager_->Load("./Resources/Sound/SE_Enemy_Damage.wav");
	bossBattleBGM_ = audioManager_->Load("./Resources/Sound/BGM_BossBattle.wav");

	clearMessage_.SetFormat("./Resources/Font/mincho_size_32.spritefont");
	clearMessage_ << "クリア！！";
	clearMessage_.scale *= 5.0f;
	clearMessage_.pos = { 134.0f, 196.0f };
	clearMessage_.color = 0xff;

	hudMessage_.SetFormat("./Resources/Font/mincho_size_32.spritefont");
	hudMessage_ << " A ボタンを押してタイトルへ戻る";
	hudMessage_.scale *= 1.0f;
	hudMessage_.pos = { 288.0f, 500.0f };
	hudMessage_.color = 0xff;
}

void GameScene::Finalize() {
	waterSE_->Stop();
	clearSE_->Stop();
	playerDamageSE_->Stop();
	enemyDamageSE_->Stop();
	bossBattleBGM_->Stop();
}

void GameScene::Update() {
	camera_->Debug("camera");
	clearMessage_.Debug("clearMessage_");
	hudMessage_.Debug("hudMessage_");

	if (player_->IsGameOver()) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}
	//water_->Debug("water");

#ifdef _DEBUG
	if (!isDebugCamera_) {
#endif // _DEBUG

		if (!isGameClear_) {
			player_->Move();
			//player_->Debug("player");
			player_->Update(*camera_);


			enemy_->Debug("Boss");
			enemy_->Update(*player_, *camera_);


			player_->Attack(*enemy_);

			if (player_->Collision(*enemy_)) {
				playerDamageSE_->Start(1.0f, false);
			}
			if (enemy_->Collision(*player_)) {
				enemyDamageSE_->Start(0.5f,false);
			}

			camera_->rotate.y = player_->GetRotate();

		}
#ifdef _DEBUG
	}
#endif // _DEBUG



#ifdef _DEBUG
	if(input_->GetInstance()->GetKey()->Pushed(DIK_TAB)){
		isDebugCamera_ = !isDebugCamera_;
	}

	if (!isDebugCamera_) {
		camera_->Update(player_->GetPos());
	}
	else {
		camera_->Update();
	}
#else
	camera_->Update(player_->GetPos());
#endif // _DEBUG

	water_->Update(camera_->GetPos());

	cloud_->Update();
	skydome_->Upadate();

	//startMessage_->Debug("startMessage_");
	if (0.0f < messageAlpah_) {
		messageAlpah_ -= 0.2f * Lamb::DeltaTime();
		if (messageAlpah_ < 0.0f) {
			bossBattleBGM_->Start(0.3f, true);
		}
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
		bossBattleBGM_->Stop();
		if (!isGameClear_) {
			clearSE_->Start(1.0f, true);
		}
		isGameClear_ = true;

		hudMessage_.color = static_cast<uint32_t>(std::abs(std::cos(messageAlpha_) * 255.0f));
		messageAlpha_ += std::numbers::pi_v<float> * 0.5f * Lamb::DeltaTime();
	}

	if (isGameClear_ && input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Title);
	}

}

void GameScene::Draw() {
	meshManager_->ResetDrawCount();

	cloud_->Draw();
	skydome_->Draw(*camera_);

	water_->Draw(camera_->GetViewProjection());

	player_->Draw(*camera_);

	if (!isGameClear_) {
		enemy_->Draw(*camera_);

		meshManager_->Draw();

		player_->AfterDraw();

		enemy_->AfterDraw(*camera_);

		startMessage_->Draw(uiCamera_->GetViewOthographics());
	}


	if (isGameClear_) {
		clearMessage_.Draw();
		hudMessage_.Draw();
	}
}