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
}

void TitleScene::Finalize()
{
	
}

void TitleScene::Update()
{
	currentCamera_->Debug("カメラ");
	currentCamera_->Update();

	//model_->Update();

	sphere_->Debug("Sphere");
	sphere_->Update();

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}

#ifdef _DEBUG
	ImGui::Begin("水の色");
	ImGui::ColorEdit4("色", color_.m.data());
	ImGui::End();
#endif // _DEBUG

}

void TitleScene::Draw()
{
	sphere_->Draw(currentCamera_->GetViewProjection(), std::numeric_limits<uint32_t>::max());

	watertsetUgoitekure_->Draw(
		waterPos_.GetMatrix(),
		currentCamera_->GetViewProjection(),
		{ random_.x, random_.y },
		color_.GetColorRGBA(),
		BlendType::kNormal
	);

	Lamb::screenout << "Model scene" << Lamb::endline
		<< "Press space to change ""Water and cloud scene""";
}
