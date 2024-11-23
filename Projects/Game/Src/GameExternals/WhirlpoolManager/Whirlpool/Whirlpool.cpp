#include "Whirlpool.h"

#include <numbers>

#include "GameExternals/CollisionManager/Collision/Collision.h"
#include "GameExternals/CameraManager/CameraManager.h"
#include "GameExternals/Player/Player.h"
#include "Camera/Camera.h"


Whirlpool::Whirlpool() {
	model_ = std::make_unique<ModelInstance>();
	model_->Load("./Resources/Block/Block.obj");
	debugModel_ = std::make_unique<ModelInstance>();
	debugModel_->Load("./Resources/DebugArrow/DebugDeath.obj");
	whirlpool_ = std::make_unique<Tex2DInstance>();
	whirlpool_->Load("./Resources/DebugArrow/DebugDeath.png");
	whirlpool_->blend = BlendType::kNormal;
	AudioManager::GetInstance()->Load("./Resources/Sound/whirlpoolSE.mp3");
	deathSE_ = AudioManager::GetInstance()->Get("./Resources/Sound/whirlpoolSE.mp3");
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
	whirlpool_->Draw(camera.GetViewProjection());
#ifdef USE_DEBUG_CODE
	Collider::DebugDraw(camera.GetViewProjection());
#endif // USE_DEBUG_CODE
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
				deathSE_->Start(0.3f, false);
			}
			player_->SetDeathAnimation(true);
			color_.at(myIndex) = Vector4ToUint(Vector4::kXIdentity);
		}
	}
}