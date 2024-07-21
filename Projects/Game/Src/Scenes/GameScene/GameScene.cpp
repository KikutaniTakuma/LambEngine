#include "GameScene.h"
#include <numbers>
#include <format>
#include "AudioManager/AudioManager.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Game/CameraManager/CameraManager.h"
#include "Game/Cloud/Cloud.h"
#include "Game/StageManager/StageManager.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include "Utils/EngineInfo/EngineInfo.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game) {}

void GameScene::Initialize() {
	blockManager_ = std::make_unique<BlockManager>();
	collisionManager_ = std::make_unique<CollisionManager>();
	customize_ = std::make_unique<Customize>();
	editorManager_ = std::make_unique<EditorManager>();
	goalManager_ = std::make_unique<GoalManager>();
	skydome_ = std::make_unique<Skydome>();
	startArea_ = std::make_unique<StartArea>();
	player_ = std::make_unique<Player>();
	windManager_ = std::make_unique<WindManager>();
	whirlpoolManager_ = std::make_unique<WhirlpoolManager>();
	water_ = Water::GetInstance();

	cursor_ = std::make_unique<Cursor>();

	camera_->farClip = 300.0f;
	camera_->pos.z = -5.0f;
	camera_->pos.y = 1.1f;
	camera_->rotate.x = 0.3f;

	blockManager_->SetPlayer(player_.get());
	blockManager_->Initialize();

	BulletManager::GetInstance()->Initialize();

	goalManager_->SetCamera(camera_.get());
	goalManager_->SetPlayer(player_.get());
	goalManager_->Initialize();

	editorManager_->Initialize(blockManager_.get(), goalManager_.get(), windManager_.get(), whirlpoolManager_.get());

	player_->SetCamera(camera_.get());

	player_->SetGoal(goalManager_->GetGoals().at(0)->GetScale(), goalManager_->GetGoals().at(0)->GetRotate(), goalManager_->GetGoals().at(0)->GetPosition());
	player_->Initialize();

	skydome_->SetPlayer(player_->GetPosition());
	skydome_->Initialize();

	startArea_->Initialize();

	windManager_->SetPlayer(player_.get());
	windManager_->Initialize();

	whirlpoolManager_->SetPlayer(player_.get());
	whirlpoolManager_->Initialize();

	customize_->SetCamera(camera_.get());
	customize_->SetPlayer(player_.get());
	customize_->Initialize();

	isDebug_ = false;

	cloud_ = Cloud::GetInstance();
	skydome_->SetTexture(cloud_->GetTex());

	miniMap_.reset(new Minimap{});
	miniMap_->Initialize(player_->GetPosition(), goalManager_->GetGoals().front()->GetPosition());

	CameraManager::GetInstance()->SetType(CameraManager::kCustomize);
	CameraManager::GetInstance()->Initialize(camera_.get(), player_.get(), player_->GetPosition(), player_->GetRotate(), goalManager_->GetGoals().at(0)->GetScale(), goalManager_->GetGoals().at(0)->GetRotate(), goalManager_->GetGoals().at(0)->GetPosition());

	customizeBGM_ = audioManager_->LoadWav("./Resources/Sound/customize.wav", true);
	customizeBGM_->Start(0.1f);

	seaSE_ = audioManager_->LoadWav("./Resources/Sound/sea.wav", true);

	clearMessage_.reset(new Texture2D{ "./Resources/InGame/goal.png" });
	clearMessage_->scale = { 588.0f, 172.0f };
	//clearMessage_->pos.y = 116.0f;
	clearMessage_->pos.x = Lamb::ClientSize().x;

	uiCamera_.reset(new Camera{});
	uiCamera_->Update();

	cursor_->Initialize();
}

void GameScene::Finalize() {
	miniMap_->Finalize();
	BulletManager::GetInstance()->Finalize();

	customizeBGM_->Stop();
	seaSE_->Stop();
}

void GameScene::Update() {
	camera_->Debug("camera");

	cursor_->Update();

	// この時にプレイヤーに線を引くのをやめたい
	if (player_->GetDeathAnimation() && !player_->GetIsSetDeathPos()) {
		miniMap_->SetPlayerDeathPos(player_->GetDeathPos());
		player_->SetIsSetDeathPos(true);
	}

	if (!player_->GetIsAlive()) {
		player_->Initialize();
		CameraManager::GetInstance()->GetCustomizeCamera()->Reset();
		CameraManager::GetInstance()->GetCustomizeCamera()->SetTarget(player_->GetPosition());
		CameraManager::GetInstance()->SetType(CameraManager::Type::kCustomize);
		miniMap_->SwitchCustomizeAndInGame();
	}
	// スタート押したとき
	if ((customize_->IsCustomizeEnd() ||
		player_->GetIsToCustomizeScene()) &&
		// ゴール中は受け付けない
		!player_->GetIsGoal() &&
		// 死んだときは受け付けない
		!player_->GetDeathAnimation()
		) {
		bool isCustomize = player_->GetIsCustomize();
		isCustomize ^= true;
		// GameScene->Customize
		if (isCustomize) {
			miniMap_->SetPlayerDeathPos(player_->GetPosition());
			player_->Initialize();
			CameraManager::GetInstance()->GetCustomizeCamera()->Reset();
			CameraManager::GetInstance()->GetCustomizeCamera()->SetTarget(player_->GetPosition());
			CameraManager::GetInstance()->SetType(CameraManager::Type::kCustomize);
			player_->StopInGameAudio();
			seaSE_->Stop();
			customizeBGM_->Start(0.1f);
		}
		// Customize->GameScene
		else {
			player_->SetIsCharge(true);
			CameraManager::GetInstance()->GetChargeCamera()->SetStartCamera(camera_->pos, camera_->rotate);
			CameraManager::GetInstance()->SetType(CameraManager::Type::kCharge);
			customizeBGM_->Stop();
			seaSE_->Start(0.05f);
			// 初期化
			for (auto& block : blockManager_->GetBlocks()) {
				block->SetHitFlag(false);
			}
			for (auto& goal : goalManager_->GetGoals()) {
				goal->SetIsHit(false);
			}
		}
		player_->SetIsCustomize(isCustomize);
		BulletManager::GetInstance()->Clear();
		customize_->Reset();
		miniMap_->SwitchCustomizeAndInGame();
	}

	if (player_->GetIsCustomize()) {
		customize_->Update();
		player_->AddPower();
#ifdef _DEBUG
		player_->DebugTileMap();
#endif // _DEBUG
		player_->ResourceUpdate();
		blockManager_->ResourceUpdate();
		goalManager_->ResourceUpdate();
		windManager_->ResourceUpdate();
		whirlpoolManager_->ResourceUpdate();

		miniMap_->SetPlayerState(player_->GetScale(), player_->GetRotate(), player_->GetPosition());

		windManager_->Update();
	}
	else {
		blockManager_->Update();
		for (auto& block : blockManager_->GetBlocks()) {
			for (uint32_t i = 0; i < block->GetColliderSize(); i++) {
				collisionManager_->SetCollider(block.get(), i);
			}
		}

		for (auto& bullet : BulletManager::GetInstance()->GetBullets()) {
			for (uint32_t i = 0; i < bullet->GetColliderSize(); i++) {
				collisionManager_->SetCollider(bullet.get(), i);
			}
		}

		goalManager_->Update();
		for (auto& goal : goalManager_->GetGoals()) {
			for (uint32_t i = 0; i < goal->GetColliderSize(); i++) {
				collisionManager_->SetCollider(goal.get(), i);
			}
		}

		windManager_->Update();
		for (auto& wind : windManager_->GetWinds()) {
			for (uint32_t i = 0; i < wind->GetColliderSize(); i++) {
				collisionManager_->SetCollider(wind.get(), i);
			}
		}

		whirlpoolManager_->Update();
		for (auto& whirlpool : whirlpoolManager_->GetWhirlpools()) {
			for (uint32_t i = 0; i < whirlpool->GetColliderSize(); i++) {
				collisionManager_->SetCollider(whirlpool.get(), i);
			}
		}


		player_->Update();

		miniMap_->SetPlayerState(player_->GetScale(), player_->GetRotate(), player_->GetPosition());

		BulletManager::GetInstance()->Update();

		collisionManager_->SetCollider(player_.get(), 0);

		if (player_->GetIsClear()) {
			SceneManager::GetInstance()->SceneChange(BaseScene::ID::StageSelect);
			StageManager::SetGaol(true);
		}

		collisionManager_->Update();

		player_->ResourceUpdate();
	}
	skydome_->SetPlayer(player_->GetPosition());
	skydome_->Update();

	startArea_->Update();

	CameraManager::GetInstance()->Update(player_->GetPosition(), player_->GetRotate());

	water_->Debug("Water");
	water_->Update(camera_->pos);

	cloud_->Update();

	if (!player_->GetIsGoal()) {
		miniMap_->AddPlayerPos(player_->GetPosition());
	}
	miniMap_->Debug();
	miniMap_->Update();

	clearMessage_->Debug("clearMessage");
	clearMessage_->Update();

	if (player_->GetIsGoal()) {
		clearMessage_->pos.x -= Lamb::ClientSize().x / 2.0f * Lamb::DeltaTime();
		clearMessage_->Update();
	}

#ifdef _DEBUG
	if (input_->GetInstance()->GetKey()->Pushed(DIK_TAB)) {
		isDebug_ ^= true;
		if (isDebug_) {
			CameraManager::GetInstance()->SetType(CameraManager::Type::kDebug);
		}
		else {
			CameraManager::GetInstance()->SetType(CameraManager::GetInstance()->GetPreType());
		}
	}
	if (isDebug_) {
		editorManager_->Update();
	}
#endif // _DEBUG

}

void GameScene::Draw() {
	cloud_->Draw();
	skydome_->Draw(*camera_);
	water_->Draw(camera_->GetViewProjection());
	whirlpoolManager_->Draw(*camera_);
	player_->Draw(*camera_);
	blockManager_->Draw(*camera_);
	goalManager_->Draw(*camera_);
	startArea_->Draw(*camera_);
	windManager_->Draw(*camera_);
	BulletManager::GetInstance()->Draw(*camera_);
	player_->DrawUI(*camera_);
	if (player_->GetIsCustomize()) {
		customize_->Draw(*camera_);
		// 上に描画したい
		player_->DrawUI(*camera_);
		//miniMap_->Draw(player_->GetPosition());
		customize_->PostDraw(*camera_);
	}
	if (player_->GetIsCustomize() || player_->GetIsStart()) {
		miniMap_->Draw(player_->GetPosition());
	}

	if (player_->GetIsGoal()) {
		clearMessage_->Draw(uiCamera_->GetViewOthographics(), Pipeline::Normal, false);
	}
#ifdef _DEBUG
	if (isDebug_) {
		editorManager_->Draw(*camera_);
	}
#endif // _DEBUG
	if (!CameraManager::GetInstance()->GetFollowCamera()->IsUsedCamera()) {
		cursor_->Draw(*camera_);
	}
	else {
		Input::GetInstance()->GetMouse()->SetPos(CameraManager::GetInstance()->GetFollowCamera()->GetPrePos());
	}
}