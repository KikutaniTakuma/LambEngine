#include "PrimitiveScene.h"
#include "Game/Water/Water.h"
#include <cmath>
#include <numbers>
#include "Utils/EngineInfo/EngineInfo.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut/ScreenOut.h"

PrimitiveScene::PrimitiveScene() :
	BaseScene{ BaseScene::ID::Primitive }
{
}

void PrimitiveScene::Initialize()
{
	camera_->pos.z = -10.0f;

	collider_.reset(new Collider());
	collider_->scale_ = Vector3::kIdentity * 1.0f;
	collider_->collisionPos_.x = 1.5f;

	sphere_.reset(new Sphere);
	sphere_->translation.x = -1.5f;
}

void PrimitiveScene::Finalize()
{

}

void PrimitiveScene::Update()
{
	camera_->Debug("カメラ");
	camera_->Update(Vector3::kZero);

	collider_->Debug("箱");
	collider_->UpdateCollision();

	sphere_->Debug("球");
	sphere_->Update();

	if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
		sceneManager_->SceneChange(BaseScene::ID::Game);
	}
}

void PrimitiveScene::Draw()
{
	collider_->DebugDraw(camera_->GetViewProjection());

	sphere_->Draw(camera_->GetViewProjection(), std::numeric_limits<uint32_t>::max());

	Line::Draw(
		Vector3::kYIdentity,
		-Vector3::kYIdentity, 
		camera_->GetViewProjection(),
		std::numeric_limits<uint32_t>::max()
		);

	Lamb::screenout << "Model scene" << Lamb::endline
		<< "Press space to change ""Water and cloud scene""";
}
