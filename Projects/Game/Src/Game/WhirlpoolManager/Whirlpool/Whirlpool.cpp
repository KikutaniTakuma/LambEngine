#include "Whirlpool.h"

#include <numbers>

#include "Game/CollisionManager/Collision/Collision.h"
#include "Game/CameraManager/CameraManager.h"
#include "Game/Player/Player.h"
#include "Camera/Camera.h"
#include "Drawers/DrawerManager.h"


Whirlpool::Whirlpool() {
	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel("./Resources/Block/Block.obj");
	drawerManager->LoadTexture("./Resources/DebugArrow/DebugDeath.png");
	AudioManager::GetInstance()->Load("./Resources/Sound/whirlpoolSE.wav");

	model_ = drawerManager->GetModel("./Resources/Block/Block.obj");
	modelInstance_ = std::make_unique<Model::Instance>();

	tex2d_ = drawerManager->GetTexture2D();
	whirlpool_ = std::make_unique<Texture2D::Instance>();
	whirlpool_->textureID = drawerManager->GetTexture("./Resources/DebugArrow/DebugDeath.png");
	deathSE_ = AudioManager::GetInstance()->Get("./Resources/Sound/whirlpoolSE.wav");
}

void Whirlpool::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos, float gameOverSpeed) {
	modelInstance_->transform.scale = scale;
	modelInstance_->transform.rotate = rotate;
	modelInstance_->transform.translate = pos;
	gameOverSpeed_ = gameOverSpeed;
	whirlpool_->transform.translate = pos;
	whirlpool_->transform.scale = { scale.x * 2.0f,scale.z * 2.0f };
	whirlpool_->transform.rotate = Quaternion::MakeRotateXAxis(1.57f);
	Collider::SetColliderType(Collider::Type::kTrriger);
	Collider::SetColliderAttribute(Collider::Attribute::kOther);
	Collider::InitializeCollision(modelInstance_->transform.scale, modelInstance_->transform.rotate, modelInstance_->transform.translate);
}

void Whirlpool::ResourceUpdate() {
	modelInstance_->transform.CalcMatrix();
	whirlpool_->transform.CalcMatrix();
}

void Whirlpool::Update() {
	color_.at(0) = Vector4ToUint(Vector4::kIdentity);
	whirlpool_->transform.rotate *= Quaternion::MakeRotateYAxis(0.01f);
	ResourceUpdate();
	Collider::UpdateCollision(modelInstance_->transform.rotate, modelInstance_->transform.translate);
}

void Whirlpool::Draw(const Camera& camera) {
	//model_->Draw(camera.GetViewProjection(), camera.GetPos());
	//debugModel_->Draw(camera.GetViewProjection(), camera.GetPos());
	tex2d_->Draw(
		whirlpool_->transform.GetMatrix(),
		Mat4x4::kIdentity,
		camera.GetViewProjection(),
		whirlpool_->textureID,
		whirlpool_->color,
		BlendType::kNormal
	);
#ifdef _DEBUG
	Collider::DebugDraw(camera.GetViewProjection());
#endif // _DEBUG
}

void Whirlpool::SetScale(const Vector3& scale) {
	modelInstance_->transform.scale = scale;
	Collider::SetCollisionScale(scale);
	Collider::UpdateCollision(modelInstance_->transform.rotate, modelInstance_->transform.translate);
}

void Whirlpool::SetRotate(const Vector3& rotate) {
	modelInstance_->transform.rotate = rotate;
	Collider::UpdateCollision(modelInstance_->transform.rotate, modelInstance_->transform.translate);
}

void Whirlpool::SetPosition(const Vector3& pos) {
	modelInstance_->transform.translate = pos;
	Collider::UpdateCollision(modelInstance_->transform.rotate, modelInstance_->transform.translate);
}

void Whirlpool::OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) {
	if (collider->GetColliderAttribute(pairIndex) == Collider::Attribute::kPlayer &&
		Collision::IsCollision(obb_.at(myIndex), collider->GetOBB(pairIndex))) {
		if (player_->GetSumVelocity() <= gameOverSpeed_) {
			if (!player_->GetDeathAnimation()) {
				player_->SetDeathPosition(player_->GetPosition(), modelInstance_->transform.translate, modelInstance_->transform.scale);
				deathSE_->Start(0.3f, false);
			}
			player_->SetDeathAnimation(true);
			color_.at(myIndex) = Vector4ToUint(Vector4::kXIdentity);
		}
	}
}