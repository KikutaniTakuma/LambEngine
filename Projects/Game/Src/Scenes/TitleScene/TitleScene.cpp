#include "TitleScene.h"
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

TitleScene::TitleScene():
	BaseScene{BaseScene::ID::Title}
{
}

void TitleScene::Load()
{
}

void TitleScene::Initialize()
{
	//currentCamera_->pos.y = 2.85f;
	currentCamera_->offset.y = -0.7f;
	currentCamera_->offset.z = -55.0f;
	currentCamera_->rotate.x = 0.37f;

	
	currentCamera_->Update();

	watertsetUgoitekure_ = std::make_unique<WaterTex2D>();
	watertsetUgoitekure_->Load();
	watertsetUgoitekure_->SetLight(
		Light{
			.ligDirection = -Vector3::kYIdentity,
			.ligColor = Vector3::kIdentity,
			.eyePos = currentCamera_->GetPos(),
			.ptPos = Vector3(0.0, 10.0f, 10.0f),
			.ptColor = Vector3::kIdentity * 15.0f
		}
	);

	random_ = Lamb::Random(Vector3::kZero, Vector3::kIdentity);

	water_ = Water::GetInstance();

	Transform playerTransform;
	playerTransform.translate = Vector3::kYIdentity * 30.0f;
	player_ = std::make_unique<Player>();
	player_->Init(playerTransform);


	Transform skyTransform;
	skyTransform.scale = Vector3{ 3.0f,1.0f,3.0f };
	skyTransform.translate = Vector3::kYIdentity * 25.0f;
	skyBlock_ = std::make_unique<SkyBlock>();
	skyBlock_->Init(skyTransform);

	coin_ = std::make_unique<Coin>();
	Transform coinTransform;
	coinTransform.scale *= 2.0f;
	coin_->Init(coinTransform);
}

void TitleScene::Finalize()
{
	
}

void TitleScene::Update()
{
	currentCamera_->Debug("カメラ");

	player_->Update();

	coin_->Update();

	skyBlock_->Debug("tset");
	skyBlock_->Update();


	/*if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}*/

	Vector3 pushVector;
	bool isCollision = skyBlock_->GetObb().IsCollision(player_->GetObb(), pushVector);
	if (isCollision) {
		player_->AfterCollisionUpdate(pushVector);
		if (player_->GetIsPunch().OnExit()) {
			skyBlock_->StartFall();
		}
	}

	player_->Landing(isCollision);

	coin_->SetIsCollision(coin_->GetObb().IsCollision(player_->GetObb()));


#ifdef _DEBUG
	if (not isDebug_) {
		currentCamera_->Update(player_->GetTranslate());
	}
	else {
		currentCamera_->Update();
	}
#else
	currentCamera_->Update(player_->GetTranslate());
#endif // DEBUG
#ifdef _DEBUG
	ImGui::Begin("水の色");
	ImGui::ColorEdit4("色", color_.m.data());
	ImGui::End();
#endif // _DEBUG


	water_->Update(currentCamera_->GetPos());
	postEffectManager_->GetPera().Debug("pera");
	postEffectManager_->GetPera().Update();
}

void TitleScene::Draw()
{
	water_->Draw(currentCamera_->GetViewProjection(), player_->GetIsPunch()  ? &postEffectManager_->GetPera(): nullptr);

	skyBlock_->Draw(*currentCamera_);

	player_->Draw(*currentCamera_);

	coin_->Draw(*currentCamera_);

	if (player_->GetIsPunch()) {
		postEffectManager_->GetPera().Draw(
			postEffectManager_->GetPeraCamera().GetViewOthographics(),
			Pipeline::Blend::Normal, nullptr, false);
		postEffectManager_->GetPera().PreDraw();
		sceneManager_->AllDraw();
		postEffectManager_->GetPera().Draw(
			postEffectManager_->GetPeraCamera().GetViewOthographics(),
			Pipeline::Blend::Normal, nullptr, false);
	}

	Lamb::screenout << "Model scene" << Lamb::endline
		<< "Press space to change ""Water and cloud scene""";
}
