#include "TitleScene.h"
#include "Game/Water/Water.h"
#include <cmath>
#include <numbers>
#include "Utils/EngineInfo/EngineInfo.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut/ScreenOut.h"

#include "Utils/Random/Random.h"
#include "imgui.h"
#include "Game/Water/Water.h"

TitleScene::TitleScene():
	BaseScene{BaseScene::ID::Title}
{
}

void TitleScene::Initialize()
{
	//currentCamera_->pos.y = 2.85f;
	currentCamera_->pos.z = -10.0f;
	//currentCamera_->rotate.x = 0.21f;

	/*model_->light.ptRange = 5.0f;
	model_->light.ptPos = model_->pos;
	model_->light.ptPos.y = 3.8f;
	model_->light.ptColor = Vector3::kIdentity * 15.0f;*/
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

	sphere_.reset(new Sphere);

	water_ = Water::GetInstance();

	player_ = std::make_unique<Player>();
	player_->Init(Transform());


	skyBlock_ = std::make_unique<SkyBlock>();
	skyBlock_->Init(Transform());
}

void TitleScene::Finalize()
{
	
}

void TitleScene::Update()
{
	currentCamera_->Debug("カメラ");

	sphere_->Debug("Sphere");
	sphere_->Update();

	skyBlock_->Debug("tset");
	skyBlock_->Update();

	player_->Update();

	/*if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}*/

	Vector3 pushVector;
	bool isCollision = skyBlock_->GetObb().IsCollision(player_->GetObb(), pushVector);
	if (isCollision) {
		player_->AfterCollisionUpdate(pushVector);
	}

	player_->Landing(isCollision);

	/*obb1.Update();
	obb2.Update();

	obb1.Debug("obb1");
	obb2.Debug("obb2");

	Vector3 hoge;
	if (obb1.IsCollision(obb2, hoge)) {
		obb2.transform.translate += hoge;
		obb2.Update();
	}*/


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
}

void TitleScene::Draw()
{
	water_->Draw(currentCamera_->GetViewProjection());

	sphere_->Draw(currentCamera_->GetViewProjection(), std::numeric_limits<uint32_t>::max());


	/*watertsetUgoitekure_->Draw(
		waterPos_.GetMatrix(),
		currentCamera_->GetViewProjection(),
		{ random_.x, random_.y },
		color_.GetColorRGBA(),
		BlendType::kNormal
	);*/


	skyBlock_->Draw(*currentCamera_);

	player_->Draw(*currentCamera_);

	/*obb1.Draw(currentCamera_->GetViewProjection());
	obb2.Draw(currentCamera_->GetViewProjection());*/


	Lamb::screenout << "Model scene" << Lamb::endline
		<< "Press space to change ""Water and cloud scene""";
}
