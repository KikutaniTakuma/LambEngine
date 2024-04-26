#include "Bullet.h"
#include "./Camera/Camera.h"
#include "imgui.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Drawers/DrawerManager.h"


void Bullet::Initialize()
{
	Lamb::SafePtr draweManager = DrawerManager::GetInstance();
	draweManager->LoadModel("./Resources/Ball.obj");
	model_ = draweManager->GetModel("./Resources/Ball.obj");
	
	isActive_ = false;

	radius_ = 1.0f;
}

void Bullet::Update()
{
	if (isActive_) {
		transform_.scale = Vector3::kIdentity * radius_;

		transform_.translate += moveDirection_ * speed_ * Lamb::DeltaTime();
	}
}

void Bullet::Draw(const Camera& camera)
{
	if (isActive_) {
		model_->Draw(
			transform_.GetMatrix(),
			camera.GetViewProjection(),
			color_,
			BlendType::kNormal
			);
	}
}

void Bullet::Debug([[maybe_unused]]const std::string& guiName)
{
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());

	ImGui::End();
#endif // _DEBUG
}

bool Bullet::CollisionBullet(const Bullet& bullet)
{
	return (transform_.translate - bullet.transform_.translate).Length() < (radius_ + bullet.radius_);
}

bool Bullet::CollisionBullet(const Vector3& pos, float radius)
{
	return (transform_.translate - pos).Length() < (radius_ + radius);
}

void Bullet::SetStatus(
	const Vector3& pos, 
	const Vector3& moveDirection, 
	float speed, 
	float attack,
	uint32_t color,
	float radius)
{
	transform_.translate = pos;
	color_ = color;

	moveDirection_ = moveDirection.Normalize();
	speed_ = speed;

	attack_ = attack;

	radius_ = radius;
}
