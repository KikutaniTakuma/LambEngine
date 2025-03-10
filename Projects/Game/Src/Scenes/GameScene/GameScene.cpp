#include "GameScene.h"
#include <numbers>
#include <format>
#include "AudioManager/AudioManager.h"
#include "GameExternals/CameraManager/CameraManager.h"
#include "GameExternals/StageManager/StageManager.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut.h"
#include "Utils/EngineInfo.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game) {}

void GameScene::Load()
{
}

void GameScene::Initialize() {
	blockManager_ = std::make_unique<BlockManager>();
	collisionManager_ = std::make_unique<CollisionManager>();
	customize_ = std::make_unique<Customize>();
	editorManager_ = std::make_unique<EditorManager>();
	goalManager_ = std::make_unique<GoalManager>();
	startArea_ = std::make_unique<StartArea>();
	player_ = std::make_unique<Player>();
	windManager_ = std::make_unique<WindManager>();
	whirlpoolManager_ = std::make_unique<WhirlpoolManager>();
	water_ = Water::GetInstance();

	cursor_ = std::make_unique<Cursor>();

	currentCamera_->farClip = 1000.0f;
	currentCamera_->pos.z = -5.0f;
	currentCamera_->pos.y = 1.1f;
	currentCamera_->rotate.x = 0.3f;

	blockManager_->SetPlayer(player_.get());
	blockManager_->Initialize();

	BulletManager::GetInstance()->Initialize();

	goalManager_->SetCamera(currentCamera_.get());
	goalManager_->SetPlayer(player_.get());
	goalManager_->Initialize();

	editorManager_->Initialize(blockManager_.get(), goalManager_.get(), windManager_.get(), whirlpoolManager_.get());

	player_->SetCamera(currentCamera_.get());

	player_->SetGoal(goalManager_->GetGoals().at(0)->GetScale(), goalManager_->GetGoals().at(0)->GetRotate(), goalManager_->GetGoals().at(0)->GetPosition());
	player_->Initialize();

	startArea_->Initialize();

	windManager_->SetPlayer(player_.get());
	windManager_->Initialize();

	whirlpoolManager_->SetPlayer(player_.get());
	whirlpoolManager_->Initialize();

	customize_->SetCamera(currentCamera_.get());
	customize_->SetPlayer(player_.get());
	customize_->Initialize();

	isDebug_ = false;



	CameraManager::GetInstance()->SetType(CameraManager::kCustomize);
	CameraManager::GetInstance()->Initialize(currentCamera_.get(), player_.get(), player_->GetPosition(), player_->GetRotate(), goalManager_->GetGoals().at(0)->GetScale(), goalManager_->GetGoals().at(0)->GetRotate(), goalManager_->GetGoals().at(0)->GetPosition());

	audioManager_->Load("./Resources/Sound/customize.mp3");
	customizeBGM_ = audioManager_->Get("./Resources/Sound/customize.mp3");
	customizeBGM_->Start(0.1f, true);

	audioManager_->Load("./Resources/Sound/sea.mp3");
	seaSE_ = audioManager_->Get("./Resources/Sound/sea.mp3");

	clearMessage_.reset(new Tex2DInstance());
	clearMessage_->Load("./Resources/InGame/goal.png");
	clearMessage_->scale = { 588.0f, 172.0f };
	//clearMessage_->pos.y = 116.0f;
	clearMessage_->pos.x = Lamb::ClientSize().x;

	uiCamera_.reset(new Camera{});
	uiCamera_->pos.z = -10.0f;
	uiCamera_->Update();

	cursor_->Initialize();

	corals_.Init();

	//renderingManager_->SetTime(12.0f);

	fishes_ = std::make_unique<Fishes>();
	fishes_->Init(200);

	tex2DData_.blend = BlendType::kNone;
	tex2DData_.uvTransform = float32_t4x4::MakeAffin(float32_t3::kIdentity * 10.0f, Quaternion::kIdentity, float32_t3::kZero);
	tex2DData_.worldMatrix = float32_t4x4::MakeAffin(float32_t3(400.f, 400.0f, 1.0f), Quaternion::MakeRotateXAxis(1.57f), water_->transform.translate);
	tex2DData_.worldMatrix[3][1] = -8.0f;
	tex2DData_.color = 0xffffffff;
	drawerManager_->LoadTexture("./Resources/sandybeach.png");
	tex2DData_.textureID = drawerManager_->GetTexture("./Resources/sandybeach.png");

	water_->StopWave();
}

void GameScene::Finalize() {
	BulletManager::GetInstance()->Finalize();

	customizeBGM_->Stop();
	seaSE_->Stop();
}

void GameScene::Update() {
	currentCamera_->Debug("camera");

	cursor_->Update();

	// この時にプレイヤーに線を引くのをやめたい
	if (player_->GetDeathAnimation() && !player_->GetIsSetDeathPos()) {
		player_->SetIsSetDeathPos(true);
	}

	if (!player_->GetIsAlive()) {
		player_->Initialize();
		CameraManager::GetInstance()->GetCustomizeCamera()->Reset();
		CameraManager::GetInstance()->GetCustomizeCamera()->SetTarget(player_->GetPosition());
		CameraManager::GetInstance()->SetType(CameraManager::Type::kCustomize);
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
			player_->Initialize();
			CameraManager::GetInstance()->GetCustomizeCamera()->Reset();
			CameraManager::GetInstance()->GetCustomizeCamera()->SetTarget(player_->GetPosition());
			CameraManager::GetInstance()->SetType(CameraManager::Type::kCustomize);
			player_->StopInGameAudio();
			seaSE_->Stop();
			customizeBGM_->Start(0.1f, true);
		}
		// Customize->GameScene
		else {
			player_->SetIsCharge(true);
			CameraManager::GetInstance()->GetChargeCamera()->SetStartCamera(currentCamera_->pos, currentCamera_->rotate);
			CameraManager::GetInstance()->SetType(CameraManager::Type::kCharge);
			customizeBGM_->Stop();
			seaSE_->Start(0.05f, true);
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
	}

	if (player_->GetIsCustomize()) {
		customize_->Update();
		player_->AddPower();
#ifdef USE_DEBUG_CODE
		player_->DebugTileMap();
#endif // USE_DEBUG_CODE
		player_->ResourceUpdate();
		blockManager_->ResourceUpdate();
		goalManager_->ResourceUpdate();
		windManager_->ResourceUpdate();
		whirlpoolManager_->ResourceUpdate();


		windManager_->Update();

		water_->StopWave();
	}
	else {
		if (not player_->GetIsCharge()) {
			water_->StartWave();
		}

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


		BulletManager::GetInstance()->Update();

		collisionManager_->SetCollider(player_.get(), 0);

		if (player_->GetIsClear()) {
			SceneManager::GetInstance()->SceneChange(BaseScene::ID::StageSelect);
			StageManager::SetGaol(true);
		}

		collisionManager_->Update();

		player_->ResourceUpdate();

	}

	startArea_->Update();

	CameraManager::GetInstance()->Update(player_->GetPosition(), player_->GetRotate());

	water_->Debug("Water");

	water_->Update();


	if (player_->GetIsGoal()) {
		clearMessage_->pos.x -= Lamb::ClientSize().x / 2.0f * Lamb::DeltaTime();
	}


#ifdef USE_DEBUG_CODE
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
#endif // USE_DEBUG_CODE

	renderingManager_->SetCameraPos(currentCamera_->GetPos());
	renderingManager_->SetViewMatrix(currentCamera_->GetView());
	renderingManager_->SetProjectionMatrix(currentCamera_->GetProjection());
	renderingManager_->Debug("randeringManager");

	water_->SetCameraPos(player_->GetPosition());

	fishes_->Update();
}

void GameScene::Draw() {
	water_->Draw(currentCamera_->GetViewProjection());
	whirlpoolManager_->Draw(*currentCamera_);
	player_->Draw(*currentCamera_);
	blockManager_->Draw(*currentCamera_);
	goalManager_->Draw(*currentCamera_);
	startArea_->Draw(*currentCamera_);
	windManager_->Draw(*currentCamera_);
	BulletManager::GetInstance()->Draw(*currentCamera_);
	player_->DrawUI(*uiCamera_);
	corals_.Draw(currentCamera_->GetViewProjection());
	fishes_->Draw(currentCamera_->GetViewProjection());

	if (player_->GetIsCustomize()) {
		customize_->Draw(*currentCamera_);
		// 上に描画したい
		player_->DrawUI(*uiCamera_);
		//miniMap_->Draw(player_->GetPosition());
		customize_->PostDraw(*uiCamera_);
	}

	if (player_->GetIsGoal()) {
		clearMessage_->blend = BlendType::kUnenableDepthNormal;
		clearMessage_->Draw(uiCamera_->GetViewOthographics());
	}
#ifdef USE_DEBUG_CODE
	if (isDebug_) {
		editorManager_->Draw(*currentCamera_);
	}
#endif // USE_DEBUG_CODE
	if (!CameraManager::GetInstance()->GetFollowCamera()->IsUsedCamera()) {
		cursor_->Draw(Mat4x4::MakeTranslate({0.0f, 0.0f, 10.0f}) * Camera::GetStaticViewOthographics());
	}
	else {
		Input::GetInstance()->GetMouse()->SetPos(CameraManager::GetInstance()->GetFollowCamera()->GetPrePos());
	}

	tex2DData_.camera = currentCamera_->GetViewProjection();
	drawerManager_->GetTexture2D()->Draw(tex2DData_);
}