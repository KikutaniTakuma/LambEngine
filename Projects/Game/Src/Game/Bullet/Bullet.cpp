#include "Bullet.h"
#include "Utils/Camera/Camera.h"
#include "imgui.h"
#include "Utils/EngineInfo/EngineInfo.h"


void Bullet::Initialize()
{
	model_.reset(new Model{ "./Resources/Ball.obj" });
	
	isActive_ = false;

	radius_ = 1.0f;
}

void Bullet::Update()
{
	if (isActive_) {
		model_->scale = Vector3::kIdentity * radius_;

		model_->pos += moveDirection_ * speed_ * Lamb::DeltaTime();

		model_->Update();
	}
}

void Bullet::Draw(const Camera& camera)
{
	if (isActive_) {
		model_->InstancingDraw(camera.GetViewProjection(), camera.GetPos());
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
	return (model_->pos - bullet.model_->pos).Length() < (radius_ + bullet.radius_);
}

bool Bullet::CollisionBullet(const Vector3& pos, float radius)
{
	return (model_->pos - pos).Length() < (radius_ + radius);
}

void Bullet::SetStatus(
	const Vector3& pos, 
	const Vector3& moveDirection, 
	float speed, 
	float attack,
	uint32_t color,
	float radius)
{
	model_->pos = pos;
	model_->color = color;

	moveDirection_ = moveDirection.Normalize();
	speed_ = speed;

	attack_ = attack;

	radius_ = radius;
}
