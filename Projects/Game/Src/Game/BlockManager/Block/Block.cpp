#include "Block.h"

#include "imgui.h"

#include "Game/CollisionManager/Collision/Collision.h"
#include "Game/Player/Player.h"
#include "Camera/Camera.h"


Block::Block() {
	model_ = std::make_unique<ModelInstance>();
	model_->Load("./Resources/InGame/Models/wall.obj");
	AudioManager::GetInstance()->Load("./Resources/Sound/hitRock.wav");
	hitBlockSound_ = AudioManager::GetInstance()->Get("./Resources/Sound/hitRock.wav");
}

void Block::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos) {
	model_->scale = scale;
	model_->rotate = rotate;
	model_->pos = pos;
	Collider::SetColliderType(Collider::Type::kCollion);
	Collider::SetColliderAttribute(Collider::Attribute::kOther);
	Collider::InitializeCollision(model_->scale, model_->rotate, model_->pos);
	subtractionVelocity_ = -2.5f;
	subtractionCannonVelocity_ = -30.0f;
	subtractionSailVelocity_ = -30.0f;
}

void Block::ResourceUpdate() {
	model_->Update();
}

void Block::Update() {
	color_.at(0) = Vector4ToUint(Vector4::kIdentity);
	ResourceUpdate();
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Block::Draw(const Camera& camera) {
	model_->Draw(camera.GetViewProjection());
#ifdef USE_IMGUI
	ImGui::Begin("Block");
	ImGui::DragFloat("Velocity", &subtractionVelocity_, 0.1f);
	ImGui::DragFloat("CannonVelocity", &subtractionCannonVelocity_, 0.1f);
	ImGui::DragFloat("SailVelocity", &subtractionSailVelocity_, 0.1f);
	ImGui::End();
	Collider::DebugDraw(camera.GetViewProjection());
#endif // USE_IMGUI
}

void Block::SetPosition(const Vector3& pos) {
	model_->pos = pos;
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Block::SetScale(const Vector3& scale) {
	model_->scale = scale;
	Collider::SetCollisionScale(scale);
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Block::SetRotate(const Vector3& rotate) {
	model_->rotate = rotate;
	Collider::UpdateCollision(model_->rotate, model_->pos);
}

void Block::OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) {
	color_.at(myIndex) = Vector4ToUint(Vector4::kYIdentity);
	if (collider->GetColliderAttribute(pairIndex) == Collider::Attribute::kPlayer &&
		Collision::IsCollision(obb_.at(myIndex), collider->GetOBB(pairIndex))) {
		color_.at(myIndex) = Vector4ToUint(Vector4::kXIdentity);
		if (!hitFlag_) {
			hitBlockSound_->Start(0.1f, false);
			player_->SetVelocity(subtractionVelocity_);
			player_->SetCannonVelocity(subtractionCannonVelocity_);
			player_->SetSailVelocity(subtractionSailVelocity_);
			hitFlag_ = true;
		}
	}
}
