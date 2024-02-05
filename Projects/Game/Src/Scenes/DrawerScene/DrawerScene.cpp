#include "DrawerScene.h"
#include "Game/Water/Water.h"
#include <cmath>
#include <numbers>
#include "Utils/EngineInfo/EngineInfo.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut/ScreenOut.h"

DrawerScene::DrawerScene():
	BaseScene{BaseScene::ID::Drawer }
{
}

void DrawerScene::Initialize()
{
	camera_->pos.z = -10.0f;

	model_.reset(new Model{ "./Resources/Ball.obj" });

	model_->light.ptRange = 5.0f;
	model_->light.ptPos = model_->pos;
	model_->light.ptPos.y = 3.8f;
	model_->light.ptColor = Vector3::kIdentity * 15.0f;

	tex2D_.reset(new Texture2D{});
	tex2D_->scale *= 256.0f;
	tex2D_->pos.x = 400.0f;

	particle_.reset(new Particle{});
	particle_->LoadSettingDirectory("demo");
	particle_->ParticleStart();
	particle_->emitterPos.x = -400.0f;

	camera2D_.reset(new Camera{});
	camera2D_->Update();
	camera2D_->pos.z = -5.0f;
}

void DrawerScene::Finalize()
{
	
}

void DrawerScene::Update()
{
	camera_->Debug("3Dモデル用カメラ");
	camera_->Update(Vector3::kZero);

	camera2D_->Debug("板ポリとか用のカメラ");
	camera2D_->Update();

	model_->Debug("テスト用モデル");
	model_->Update();

	tex2D_->Debug("テスト用板ポリ");
	tex2D_->Update();

	particle_->Debug("テスト用パーティクル");
	particle_->Update();

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Primitive);
	}
}

void DrawerScene::Draw()
{
	model_->Draw(camera_->GetViewProjection(), camera_->GetPos());

	tex2D_->Draw(camera2D_->GetViewOthographics());

	particle_->Draw(camera2D_->rotate, camera2D_->GetViewOthographics());

	Lamb::screenout << "Model scene" << Lamb::endline
		<< "Press space to change ""Primitive scene""";
}
