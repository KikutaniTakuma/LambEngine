#include "Enemy.h"
#include "Utils/Camera/Camera.h"
#include "imgui.h"
#include "../Player/Player.h"

#include <numbers>

void Enemy::Initialize()
{
	model_.reset(new Model{ "./Resources/Ball.obj" });

	CreateBullets();
	CreateBehaviors();

	radius_ = 12.0f;

	hp_ = 1000.0f;
}

void Enemy::Update()
{
	model_->scale = Vector3::kIdentity * radius_;
	model_->Update();
}

void Enemy::Draw(const Camera& camera)
{
	model_->Draw(camera.GetViewProjection(), camera.GetPos());
}

void Enemy::Debug([[maybe_unused]]const std::string& guiName)
{
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat("radius", &radius_);
	ImGui::Text("hp : %.0f", hp_);
	ImGui::End();

	model_->scale = Vector3::kIdentity * radius_ * 0.5f;
#endif // _DEBUG
}

void Enemy::Collision(const Player& player)
{
	const auto& playerBullets = player.GetBullets();

	for (const auto& i : playerBullets) {
		if (i->GetIsActive()) {
			if (i->CollisionBullet(model_->pos, radius_)) {
				hp_ -= i->GetAttack();
				i->Unenable();
			}
		}
	}
}


void Enemy::CreateBehaviors() {
	behavior_[Behavior::SpinningAttack] = [this]() {};
	behavior_[Behavior::CrossAttack] = [this]() {};
	behavior_[Behavior::EruptionAttack] = [this]() {};
	behavior_[Behavior::LargeEruptionAttack] = [this]() {};
	behavior_[Behavior::None] = []() {};
}

void Enemy::CreateBullets()
{
	bullets_.resize(50);

	for (auto& i : bullets_) {
		i.reset(new Bullet{});
		i->Initialize();
	}
}
