#include "Bullet.h"

#include "Game/CollisionManager/Collision/Collision.h"
#include "Camera/Camera.h"
#include "Utils/EngineInfo.h"

void Bullet::Initialize() {
	model_ = std::make_unique<ModelInstance>();
	model_->Load("./Resources/Bullet/Bullet.obj");

	Collider::SetColliderType(Collider::Type::kCollion);
	Collider::SetColliderAttribute(Collider::Attribute::kPlayerBullet);
	Collider::InitializeCollision(model_->scale, model_->rotate, model_->pos);
	Collider::SetCollisionScale(Vector3(0.5f, 0.5f, 0.5f));
}

void Bullet::Update() {
	velocity_.y -= 0.5f / 60.0f;
	model_->pos += velocity_;
	if (model_->pos.y < 0.0f) {
		isAlive_ = false;
	}
	UpdateCollision(model_->rotate, model_->pos);
	
}

void Bullet::ModelUpdate() {
	model_->Update();
}

void Bullet::Draw(const Camera& camera) {
	model_->Draw(camera.GetViewProjection());

#ifdef USE_IMGUI
	DebugDraw(camera.GetViewProjection());
#endif // USE_IMGUI
}

void Bullet::SetBullet(const Vector3& position, const Vector3& vector) {
	model_->pos = position;
	velocity_ = vector * 20.0f/60.0f;
	velocity_.y += 10.0f/60.0f;
	isAlive_ = true;
}

void Bullet::OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) {
	if (collider->GetColliderType(pairIndex) == Collider::Type::kCollion &&
		collider->GetColliderAttribute(pairIndex) == Collider::Attribute::kOther &&
		Collision::IsCollision(obb_.at(myIndex), collider->GetOBB(pairIndex))) {
		isAlive_ = false;
	}
}
