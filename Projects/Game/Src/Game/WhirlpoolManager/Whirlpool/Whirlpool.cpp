#include "Whirlpool.h"

#include <numbers>

#include "Game/CollisionManager/Collision/Collision.h"
#include "Game/CameraManager/CameraManager.h"
#include "Game/Player/Player.h"
#include "Utils/Camera/Camera.h"


Whirlpool::Whirlpool() {
	model_ = std::make_unique<Model>();
	model_->LoadObj("./Resources/Block/Block.obj");
	debugModel_ = std::make_unique<Model>();
	debugModel_->LoadObj("./Resources/DebugArrow/DebugDeath.obj");
	whirlpool_ = std::make_unique<Texture2D>();
	whirlpool_->LoadTexture("./Resources/DebugArrow/DebugDeath.png");
	deathSE_ = AudioManager::GetInstance()->LoadWav("./Resources/Sound/whirlpoolSE.wav", false);
}

void Whirlpool::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos, float gameOverSpeed) {
	model_->scale = scale;
	model_->rotate = rotate;
	model_->pos = pos;
	gameOverSpeed_ = gameOverSpeed;
	whirlpool_->pos = pos;
	whirlpool_->scale = { scale.x * 2.0f,scale.z * 2.0f };
	whirlpool_->rotate.x = 1.57f;
	debugModel_->SetParent(model_.get());
	Collider::SetColliderType(Collider::Type::kTrriger);
	Collider::SetColliderAttribute(Collider::Attribute::kOther);
	Collider::InitializeCollision(model_->scale, model_->rotate, model_->pos);
}

void Whirlpool::ResourceUpdate() {
	model_->Update();
	debugModel_->Update();
	whirlpool_->Update();
}

void Whirlpool::Update() {
	color_.at(0) = Vector4ToUint(Vector4::kIdentity);
	whirlpool_->rotate.y += 0.01f;
	ResourceUpdate();
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Whirlpool::Draw(const Camera& camera) {
	//model_->Draw(camera.GetViewProjection(), camera.GetPos());
	//debugModel_->Draw(camera.GetViewProjection(), camera.GetPos());
	whirlpool_->Draw(camera.GetViewProjection(), Pipeline::Normal, false, false);
#ifdef _DEBUG
	Collider::DebugDraw(camera.GetViewProjection());
#endif // _DEBUG
}

void Whirlpool::SetScale(const Vector3& scale) {
	model_->scale = scale;
	Collider::SetCollisionScale(scale);
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Whirlpool::SetRotate(const Vector3& rotate) {
	model_->rotate = rotate;
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Whirlpool::SetPosition(const Vector3& pos) {
	model_->pos = pos;
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Whirlpool::OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) {
	if (collider->GetColliderAttribute(pairIndex) == Collider::Attribute::kPlayer &&
		Collision::IsCollision(obb_.at(myIndex), collider->GetOBB(pairIndex))) {
		if (player_->GetSumVelocity() <= gameOverSpeed_) {
			if (!player_->GetDeathAnimation()) {
				player_->SetDeathPosition(player_->GetPosition(), model_->pos, model_->scale);
				deathSE_->Start(0.3f);
			}
			player_->SetDeathAnimation(true);
			color_.at(myIndex) = Vector4ToUint(Vector4::kXIndentity);
		}
	}
}