#include "Enemy.h"
#include "./Camera/Camera.h"
#include "imgui.h"
#include "../Player/Player.h"
#include "Math/Quaternion.h"
#include "Math/MathCommon.h"

#include "Utils/EngineInfo/EngineInfo.h"
#include "Drawers/DrawerManager.h"

#include <numbers>
#include "Input/Input.h"

void Enemy::Initialize()
{
	Lamb::SafePtr draweManager = DrawerManager::GetInstance();
	draweManager->LoadModel("./Resources/Enemy/Enemy.obj");
	model_ = draweManager->GetModel("./Resources/Enemy/Enemy.obj");
	transform_.translate.y = 14.5f;
	transform_.scale = Vector3::kIdentity * 12.0f;

	CreateBullets();
	CreateBehaviors();

	radius_ = 6.0f;

	hp_ = 1000.0f;

	currentBehavior_ = Behavior::None;
	nextBehavior_ = std::nullopt;

	particle_.reset(new Particle{});
	//particle_->LoadSettingDirectory("PlayerParticle");

	particle_->ParticleStart();

	particleCamera_.reset(new Camera{});
	particleCamera_->Update();

	toPlayerLength_ = 0.0f;

	isAttackStart_ = false;

	/*uiCamera_.reset(new Camera{});
	uiCamera_->Update();
	uiFrame_.reset(new Texture2D{});
	uiFrame_->pos = { 357.0f, 278.0f };
	uiFrame_->scale = { 500.0f, 80.0f };
	uiFrame_->color = Vector4ToUint({ 0.1f,0.1f,0.1f,1.0f });
	uiHp_.reset(new Texture2D{});
	uiHp_->color = Vector4ToUint({ 0.8f,0.1f,0.1f,1.0f });
	uiHp_->pos = { 357.0f, 278.0f };
	uiHp_->scale = { 450.0f, 52.0f };*/


	hitColorCountBasis_ = 0.1f;
	hitColorCount_ = 0.0f;
}

void Enemy::Update(const Player& player, const Camera& camera)
{
	playerPos_ = player.GetPos();
	Vector3 posTmp = transform_.translate;
	posTmp.y += radius_;
	toPlayer_ = player.GetPos() - posTmp;
	toPlayerLength_ = toPlayer_.Length();
	toPlayer_.Normalize();

	SettingBehavior();

	if (0.0f < player.GetHp()) {
		behavior_[currentBehavior_]();
	}

	for (auto& i : bullets_) {
		i->Update();
	}

	if (0.0f < player.GetHp()) {
		behaviorTime_ += Lamb::DeltaTime();
	}

	particle_->emitterPos = transform_.translate * camera.GetViewProjectionVp() * particleCamera_->GetViewOthographicsVp().Inverse();
	particle_->Update();

	/*if (0.0f < hp_) {
		uiHp_->uvPibot.x = 1.0f - (hp_ / 1000.0f);
	}
	else {
		uiHp_->uvPibot.x = 2.0f;
	}

	uiFrame_->Update();
	uiHp_->Update();*/
}

void Enemy::Draw(const Camera& camera)
{
	model_->Draw(
		transform_.GetMatrix(),
		camera.GetViewProjection(),
		color_,
		BlendType::kNormal
		);

	for (auto& i : bullets_) {
		i->Draw(camera);
	}

}

void Enemy::AfterDraw([[maybe_unused]] const Camera& camera)
{
	particle_->Draw(Vector3::kZero, particleCamera_->GetViewOthographics());

	/*uiFrame_->Draw(uiCamera_->GetViewOthographics(), Pipeline::Normal, false);
	uiHp_->Draw(uiCamera_->GetViewOthographics(), Pipeline::Normal, false, false);*/
}

void Enemy::Debug([[maybe_unused]]const std::string& guiName)
{
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat("radius", &radius_);
	ImGui::DragFloat("hp", &hp_);
	ImGui::Text("toPlayerLength_ : %.2f", toPlayerLength_);
	ImGui::End();
#endif // _DEBUG
}

bool Enemy::Collision(const Player& player)
{
	const auto& playerBullets = player.GetBullets();

	bool isClollsion = false;

	for (const auto& i : playerBullets) {
		if (i->GetIsActive()) {
			if (i->CollisionBullet(transform_.translate, radius_)) {
				hp_ -= i->GetAttack();
				i->Unenable();
				isClollsion = true;
				color_ = 0xff0000ff;
			}
		}
	}

	if (!isClollsion && hitColorCountBasis_ < hitColorCount_) {
		color_ = 0xffffffff;
		hitColorCount_ = 0.0f;
	}

	if (color_ == 0xff0000ff) {
		hitColorCount_ += Lamb::DeltaTime();
	}

	return isClollsion;
}

void Enemy::StartAttack()
{
	if (!isAttackStart_) {
		nextBehavior_ = Behavior::OneShot;
		isAttackStart_ = true;
	}
}


void Enemy::CreateBehaviors() {
	// 攻撃
	behavior_[Behavior::OneShot] = [this]() {
		if (behaviorTime_ == 0.0f) {
			currentBullet_->get()->SetStatus(
				Vector3{ transform_.translate.x,transform_.translate.y + radius_,transform_.translate.z },
				toPlayer_,
				15.0f,
				20.0f,
				Vector4ToUint({0.8f, 0.2f, 0.2f, 1.0f})
			);
			currentBullet_->get()->Enable();

			NextBullet();
		}
		else if (behaviorFinishTime_[currentBehavior_] < behaviorTime_) {
			if (3 < oneShoeCount_) {
				oneShoeCount_ = 0;
				if (toPlayerLength_ < 33.0f) {
					if (hp_ < 500.0f) {
						nextBehavior_ = Behavior::EruptionAttack;
						return;
					}
					else {
						nextBehavior_ = Behavior::EruptionAttack;
						return;
					}
				}
				else {
					nextBehavior_ = Behavior::CrossAttack;
				}
				return;
			}
			else {
				oneShoeCount_++;
				nextBehavior_ = Behavior::OneShot;
				return;
			}
		}
		nextBehavior_ = std::nullopt;
	};
	behavior_[Behavior::CrossAttack] = [this]() {
		if (static_cast<int32_t>(std::floor(behaviorTime_ * 100.0f)) % 20 == 0) {
			float rotateCount = 0.0f;
			for (int32_t i = 0; i < 4; i++) {
				currentBullet_->get()->SetStatus(
					Vector3{ transform_.translate.x,transform_.translate.y + radius_,transform_.translate.z },
					(toPlayer_ * Quaternion::MakeRotateAxisAngle(Vector3::kYIdentity, rotateCount)).Normalize(),
					15.0f,
					20.0f,
					Vector4ToUint({ 0.8f, 0.2f, 0.2f, 1.0f })
				);

				rotateCount += std::numbers::pi_v<float> * 0.5f;

				currentBullet_->get()->Enable();

				NextBullet();
			}
		}
		if (behaviorFinishTime_[currentBehavior_] < behaviorTime_) {
			nextBehavior_ = Behavior::OneShot;
		}
		else {
			nextBehavior_ = std::nullopt;
		}
	};

	behavior_[Behavior::EruptionAttack] = [this]() {
		if (static_cast<int32_t>(std::floor(behaviorTime_ * 100.0f)) % 10 == 0) {
			currentBullet_->get()->SetStatus(
				Vector3{ playerPos_.x,playerPos_.y + transform_.translate.y,playerPos_.z },
				-Vector3::kYIdentity,
				10.0f,
				20.0f,
				Vector4ToUint({ 0.8f, 0.2f, 0.2f, 1.0f })
			);
			currentBullet_->get()->Enable();

			NextBullet();
		}
		if (behaviorFinishTime_[currentBehavior_] < behaviorTime_) {
			nextBehavior_ = Behavior::OneShot;
		}
		else {
			nextBehavior_ = std::nullopt;
		}
	};
	behavior_[Behavior::LargeEruptionAttack] = [this]() {
		if (static_cast<int32_t>(std::floor(behaviorTime_ * 100.0f)) % 5 == 0) {
			currentBullet_->get()->SetStatus(
				Vector3{ playerPos_.x,playerPos_.y + transform_.translate.y,playerPos_.z },
				-Vector3::kYIdentity,
				30.0f,
				20.0f,
				Vector4ToUint({ 0.8f, 0.2f, 0.2f, 1.0f })
			);
			currentBullet_->get()->Enable();

			NextBullet();
		}
		if (behaviorFinishTime_[currentBehavior_] < behaviorTime_) {
			nextBehavior_ = Behavior::OneShot;
		}
		else {
			nextBehavior_ = std::nullopt;
		}
	};

	// 途中行動
	behavior_[Behavior::Down] = [this]() {};
	behavior_[Behavior::Charge] = [this]() {};

	// 何もしない
	behavior_[Behavior::None] = [this]() {};


	behaviorFinishTime_[Behavior::OneShot] = 1.0f;
	behaviorFinishTime_[Behavior::CrossAttack] = 1.0f;
	behaviorFinishTime_[Behavior::EruptionAttack] = 5.0f;
	behaviorFinishTime_[Behavior::LargeEruptionAttack] = 10.0f;

	// 途中行動
	behaviorFinishTime_[Behavior::Down] = 0.0f;
	behaviorFinishTime_[Behavior::Charge] = 0.0f;

	behaviorFinishTime_[Behavior::None] = 1.5f;
}

void Enemy::CreateBullets()
{
	bullets_.resize(100);

	for (auto& i : bullets_) {
		i.reset(new Bullet{});
		i->Initialize();
	}

	currentBullet_ = bullets_.begin();
}

void Enemy::SettingBehavior()
{
	if (nextBehavior_ == std::nullopt) {
		return;
	}

	currentBehavior_ = nextBehavior_;
	behaviorTime_ = 0.0f;
}

void Enemy::NextBullet()
{
	currentBullet_++;
	if (currentBullet_ == bullets_.end()) {
		currentBullet_ = bullets_.begin();
	}
}
