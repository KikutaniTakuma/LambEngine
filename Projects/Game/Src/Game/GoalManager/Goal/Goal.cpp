#include "Goal.h"

#include <cmath>
#include <numbers>

#include "Game/CollisionManager/Collision/Collision.h"
#include "Game/Player/Player.h"
#include "Camera/Camera.h"
#include "Game/CameraManager/CameraManager.h"
#include "Utils/EngineInfo.h"
#include "Utils/Random.h"

#include "imgui.h"


Goal::Goal() {
	goal_ = std::make_unique<ModelInstance>();
	goal_->Load("./Resources/DebugArrow/DebugGoal.obj");
	leftBeach_ = std::make_unique<ModelInstance>();
	leftBeach_->Load("./Resources/InGame/Models/beach.obj");
	rightBeach_ = std::make_unique<ModelInstance>();
	rightBeach_->Load("./Resources/InGame/Models/beach.obj");
	overSize_ = 4.0f;
}

void Goal::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& pos) {
	goal_->scale = scale;
	goal_->rotate = rotate;
	goal_->pos = pos;
	leftBeach_->pos.x += goal_->scale.x * 0.5f;
	rightBeach_->pos.x -= goal_->scale.x * 0.5f;
	leftBeach_->SetParent(goal_.get());
	rightBeach_->SetParent(goal_.get());
	ResourceUpdate();
	Collider::SetColliderType(Collider::Type::kTrriger);
	Collider::SetColliderAttribute(Collider::Attribute::kOther);
	Mat4x4 tmp = Mat4x4::MakeRotateY(goal_->rotate.y);
	Collider::InitializeCollision({ goal_->scale }, goal_->rotate, goal_->pos + (Vector3{ 0.0f,0.0f,goal_->scale.z } * tmp));
	tmp = leftBeach_->GetWorldMatrix();
	Collider::SetColliderType(Collider::Type::kCollion);
	Collider::SetColliderAttribute(Collider::Attribute::kOther);
	Collider::InitializeCollision({ scale.x * overSize_ ,scale.y,scale.z }, MakeEulerAngle(tmp), { tmp[3][0],tmp[3][1],tmp[3][2] });
	tmp = rightBeach_->GetWorldMatrix();
	Collider::SetColliderType(Collider::Type::kCollion);
	Collider::SetColliderAttribute(Collider::Attribute::kOther);
	Collider::InitializeCollision({ scale.x * overSize_,scale.y,scale.z }, MakeEulerAngle(tmp), { tmp[3][0],tmp[3][1],tmp[3][2] });

	for (auto& i : particles_) {
		i.reset(new Particle{});
		i->LoadSettingDirectory("firework");
	}

	particleCoolTime_ = 0.2f;
	particleCount_ = 0.0f;
	isParticleStart_ = false;

	particleCoolTimeRange_.first = 0.4f;
	particleCoolTimeRange_.second = 0.6f;

	emitterRange_.first =  -goal_->scale * 0.4f + goal_->pos;
	emitterRange_.second  = goal_->scale * 0.4f + goal_->pos;

	emitterRange_.first.y = 5.0f;
	emitterRange_.second.y = 8.0f;

	AudioManager::GetInstance()->Load("./Resources/Sound/fireworkSE.wav");
	fireworkSound_ = AudioManager::GetInstance()->Get("./Resources/Sound/fireworkSE.wav");

	subtractionVelocity_ = -2.5f;
	subtractionCannonVelocity_ = -30.0f;
	subtractionSailVelocity_ = -30.0f;
}

void Goal::ResourceUpdate() {
	goal_->Update();
	leftBeach_->Update();
	rightBeach_->Update();
}

void Goal::Update() {
	ResourceUpdate();
	for (int i = 0; i < color_.size(); i++) {
		color_.at(i) = Vector4ToUint(Vector4::kIdentity);
	}
	UpdateCollision();
	/*ImGui::Begin("JHOHOHOIH");
	ImGui::DragFloat3("ledt", &pos_.x);
	ImGui::End();*/

	ParticleUpdate();
}

void Goal::Draw(const Camera& camera) {
	goal_->Draw(camera.GetViewProjection());
	leftBeach_->Draw(camera.GetViewProjection());
	rightBeach_->Draw(camera.GetViewProjection());

	ParticleDraw(camera);

#ifdef USE_DEBUG_CODE
	for (int i = 0; i < color_.size(); i++) {
		Collider::DebugDraw(camera.GetViewProjection(), i);
	}
#endif // USE_DEBUG_CODE
}

void Goal::SetScale(const Vector3& scale) {
	goal_->scale = scale;
	ResourceUpdate();
	Collider::SetCollisionScale(goal_->scale, 0);
	Collider::SetCollisionScale({ scale.x * overSize_, scale.y, scale.z }, 1);
	Collider::SetCollisionScale({ scale.x * overSize_ , scale.y, scale.z }, 2);
	UpdateCollision();
}

void Goal::SetRotate(const Vector3& rotate) {
	goal_->rotate = rotate;
	ResourceUpdate();
	UpdateCollision();
}

void Goal::SetPosition(const Vector3& pos) {
	goal_->pos = pos;
	ResourceUpdate();
	UpdateCollision();
}

void Goal::ParticleUpdate()
{
	if (isParticleStart_) {
		for (auto& i : particles_) {
			if (!i->GetIsParticleStart() &&
				particleCoolTime_ <= particleCount_) {
				particleCount_ = 0.0f;
				Vector3 eimtterPos = Lamb::Random(emitterRange_.first, emitterRange_.second);
				i->ParticleStart(eimtterPos);
				particleCoolTime_ = Lamb::Random(particleCoolTimeRange_.first, particleCoolTimeRange_.second);
				
				// 音鳴らす
				//fireworkSound_->Stop();
				//fireworkSound_->Start(0.5f, false);
				break;
			}
		}
		for (auto& i : particles_) {
			i->Update();
		}

		particleCount_ += Lamb::DeltaTime();
	}

}

void Goal::ParticleDraw(const Camera& camera)
{
	if (isParticleStart_) {
		/*for (auto& i : particles_) {
			i->Draw(camera.rotate, camera.GetViewProjection());
		}*/
	}
}

void Goal::OnCollision(Collider* collider, uint32_t myIndex, uint32_t pairIndex) {

	if (myIndex == 0 &&
		collider->GetColliderAttribute(pairIndex) == Collider::Attribute::kPlayer &&
		Collision::IsCollision(obb_.at(myIndex), collider->GetOBB(pairIndex)) &&
		!player_->GetIsGoal()) {
		player_->SetIsGoal(true);
		player_->SetIsStart(false);
		player_->SetGoalPlayerPos(player_->GetPosition());
		player_->SetGoalPlayerRotate(player_->GetRotate().y);
		CameraManager::GetInstance()->SetType(CameraManager::Type::kGoal);
		CameraManager::GetInstance()->GetGoalCamera()->SetStartCamera(camera_->pos, camera_->rotate);
		color_.at(myIndex) = Vector4ToUint(Vector4::kXIdentity);
		isParticleStart_ = true;
	}
	else if (
		!isHit_ &&
		myIndex != 0 &&
		collider->GetColliderAttribute(pairIndex) == Collider::Attribute::kPlayer &&
		Collision::IsCollision(obb_.at(myIndex), collider->GetOBB(pairIndex))) {
		player_->SetVelocity(subtractionVelocity_);
		player_->SetCannonVelocity(subtractionCannonVelocity_);
		player_->SetSailVelocity(subtractionSailVelocity_);
		color_.at(myIndex) = Vector4ToUint(Vector4::kXIdentity);
		isHit_ = true;
	}
}

void Goal::UpdateCollision() {
	Mat4x4 tmp = Mat4x4::MakeRotateY(goal_->rotate.y);
	Collider::UpdateCollision(goal_->rotate, goal_->pos + (Vector3{ 0.0f,0.0f,goal_->scale.z }*tmp), 0);
	tmp = leftBeach_->GetWorldMatrix();
	Collider::UpdateCollision(MakeEulerAngle(tmp), { tmp[3][0],tmp[3][1],tmp[3][2] }, 1);
	tmp = rightBeach_->GetWorldMatrix();
	Collider::UpdateCollision(MakeEulerAngle(tmp), { tmp[3][0],tmp[3][1],tmp[3][2] }, 2);
}

Vector3 Goal::MakeEulerAngle(const Mat4x4& mat) {
	Vector3 angles;

	// Extract yaw (Y軸回りの回転)
	angles.y = std::atan2(mat[2][0], mat[2][2]);

	// Extract pitch (X軸回りの回転)
	float sinPitch = -mat[2][1];
	if (std::fabs(sinPitch) >= 1.0f) {
		angles.x = std::copysign(std::numbers::pi_v<float> *0.5f, sinPitch);
	}
	else {
		angles.x = std::asin(sinPitch);
	}

	// Extract roll (Z軸回りの回転)
	angles.z = std::atan2(mat[0][1], mat[1][1]);

	return angles;
}
