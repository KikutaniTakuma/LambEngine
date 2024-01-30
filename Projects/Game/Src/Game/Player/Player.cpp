#include "Player.h"
#include "Input/Input.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "imgui.h"
#include "../Enemy/Enemy.h"

#include <algorithm>
#include <numbers>

void Player::Initialize()
{
	model_.reset(new Model{"./Resources/Player/Player.obj"});
	model_->scale *= 5.0f;

	attack_ = 4.0f;

	maxSpeed_ = std::numbers::pi_v<float> / 8.0f;
	speed_ = 0.0f;
	addSpeed_ = maxSpeed_ * 0.3f;
	speedDecay_ = -maxSpeed_ * 0.5f;

	float maxOffsetZ = 50.0f;
	float minOffsetZ = 20.0f;

	maxOffset_ = -Vector3::kZIndentity * minOffsetZ;
	minOffset_ = -Vector3::kZIndentity * maxOffsetZ;
	offset_ = -Vector3::kZIndentity * 35.0f;
	offsetSpeed_ = 5.0f;

	basisSpeedScale_ = maxOffsetZ;
	speedScale_ = 1.0f / (basisSpeedScale_ / offset_.Length());

	rotate_ = 0.0f;

	radius_ = 2.0f;
	isCollisioned_ = false;
	invincibleTime_ = 1.5f;
	isCollisionedTime_ = 0.0f;


	hp_ = 100.0f;

	isReloadable_ = false;

	CreateBullets();


	particle_.reset(new Particle{});
	particle_->LoadSettingDirectory("PlayerDeath");

	particleCamera_.reset(new Camera{});
	particleCamera_->Update();

	isParticled_ = false;


	modelUpDown_ = 0.0f;
	modelUpDownSpeed_ = std::numbers::pi_v<float>;


	uiCamera_.reset(new Camera{});
	uiCamera_->Update();
	uiFrame_.reset(new Texture2D{});
	uiFrame_->pos = { -466.0f, -279.0f };
	uiFrame_->scale = { 270.0f, 80.0f };
	uiFrame_->color = Vector4ToUint({ 0.1f,0.1f,0.1f,1.0f });
	uiHp_.reset(new Texture2D{});
	uiHp_->color = Vector4ToUint({ 0.1f,0.8f,0.1f,1.0f });
	uiHp_->pos = { -466.0f, -279.0f };
	uiHp_->scale = { 225.0f, 52.0f };

	uiBulletFrame_.reset(new Texture2D{});
	uiBulletFrame_->pos = { -466.0f, -180.0f };
	uiBulletFrame_->scale = { 270.0f, 80.0f };
	uiBulletFrame_->color = Vector4ToUint({ 0.1f,0.1f,0.1f,1.0f });

	for (size_t i = 0; i < uiBullet_.size(); i++) {
		uiBullet_[i].reset(new Texture2D{});
		uiBullet_[i]->scale = { 30.0f, 60.0f };
		uiBullet_[i]->color = Vector4ToUint({ 0.8f, 0.2f, 0.2f, 1.0f });
		uiBullet_[i]->pos.x = -565.0f + 40.0f * static_cast<float>(i);
		uiBullet_[i]->pos.y = -180.0f;
	}
	uiBullet_[5]->color = Vector4ToUint({ 0.2f, 0.8f, 0.8f, 1.0f });

	chikachika_ = 0.0f;
}

void Player::Move()
{
	if (hp_ <= 0.0f) {
		return;
	}

	static KeyInput* const key = Input::GetInstance()->GetKey();
	static Gamepad* const gamepad = Input::GetInstance()->GetGamepad();

	Vector3 move;

	const Vector2 stick = {
		gamepad->GetStick(Gamepad::Stick::LEFT_X),
		gamepad->GetStick(Gamepad::Stick::LEFT_Y) 
	};


	// オフセット移動
	if (key->GetKey(DIK_W) || key->GetKey(DIK_UP) ||
		gamepad->GetButton(Gamepad::Button::UP) || 0.0f < stick.y
		) 
	{
		offset_.z += offsetSpeed_ * Lamb::DeltaTime();
	}
	else if (key->GetKey(DIK_S) || key->GetKey(DIK_DOWN) ||
		gamepad->GetButton(Gamepad::Button::DOWN) || stick.y < 0.0f
		)
	{
		offset_.z -= offsetSpeed_ * Lamb::DeltaTime();
	}

	offset_.z = std::clamp(offset_.z, minOffset_.z, maxOffset_.z);

	speedScale_ = 1.0f / (basisSpeedScale_ / offset_.Length());

	bool isMove = true;
	// 右へ
	if (key->GetKey(DIK_D) || key->GetKey(DIK_RIGHT) || 
		gamepad->GetButton(Gamepad::Button::RIGHT) || 0.0f < stick.x) 
	{
		speed_ -= addSpeed_ * Lamb::DeltaTime() * speedScale_;
	}
	// 左へ
	else if (key->GetKey(DIK_A) || key->GetKey(DIK_LEFT) || 
		gamepad->GetButton(Gamepad::Button::LEFT) || stick.x < 0.0f) 
	{
		speed_ += addSpeed_ * Lamb::DeltaTime() * speedScale_;
	}
	else {
		isMove = false;
	}

	// 速度のクランプ
	speed_ = std::clamp(speed_, -maxSpeed_ * speedScale_, maxSpeed_ * speedScale_);

	// 動いてなかったら
	if (!isMove) {
		// 速度が0未満
		if (speed_ < 0.0f) {
			speed_ -= speedDecay_ * Lamb::DeltaTime() * speedScale_;
			// 0以上になったら0を代入
			if (0.0f <= speed_) {
				speed_ = 0.0f;
			}
		}
		// 速度が0より大きいい
		else if(0.0f < speed_){
			speed_ += speedDecay_ * Lamb::DeltaTime() * speedScale_;
			// 0以下になったら0を代入
			if (speed_ <= 0.0f) {
				speed_ = 0.0f;
			}
		}
	}
}

void Player::Update(const Camera& camera) {
	if (hp_ <= 0.0f && !particle_->GetIsParticleStart() && !isParticled_) {
		particle_->ParticleStart();
		isParticled_ = true;
	}

	rotate_ += speed_ * Lamb::DeltaTime() * speedScale_;

	model_->pos = offset_ * Quaternion::MakeRotateYAxis(rotate_);
	noUpDownPos_ = model_->pos;
	model_->rotate.y = rotate_;

	modelUpDown_ += modelUpDownSpeed_ * Lamb::DeltaTime() * (1.0f + (speed_ / (maxSpeed_ * speedScale_)) * 0.1f);
	
	model_->pos.y += std::cos(modelUpDown_) * 0.1f;

	if (isCollisioned_) {
		model_->color = Vector4ToUint({ 1.0f,1.0f,1.0f,std::abs(std::cos(chikachika_)) });
		chikachika_ += std::numbers::pi_v<float> *Lamb::DeltaTime() * 3.0f;
	}
	else {
		model_->color = Vector4ToUint({ 1.0f,1.0f,1.0f,1.0f });
		chikachika_ = 0.0f;
	}

	model_->Update();

	for (auto& i : bullets_) {
		i->Update();
	}

	if (isCollisioned_) {
		isCollisionedTime_ += Lamb::DeltaTime();
		if (invincibleTime_ < isCollisionedTime_) {
			isCollisionedTime_ = 0.0f;
			isCollisioned_ = false;
		}
	}

	particle_->emitterPos = model_->pos * camera.GetViewProjectionVp() * Mat4x4::MakeInverse(particleCamera_->GetViewOthographicsVp());
	particle_->Update();

	if (0.0f < hp_) {
		uiHp_->uvPibot.x = 1.0f - (hp_ / 100.0f);
	}
	else {
		uiHp_->uvPibot.x = 2.0f;
	}

	uiBulletFrame_->Update();

	for (size_t i = 0; i < uiBullet_.size(); i++) {
		uiBullet_[i]->Update();
	}

	uiFrame_->Update();
	uiHp_->Update();
}

void Player::Draw(const Camera& camera)
{
	if (0.0f < hp_) {
		model_->Draw(camera.GetViewProjection(), camera.GetPos());
	}

	for (auto& i : bullets_) {
		i->Draw(camera);
	}

}

void Player::AfterDraw()
{
	particle_->Draw(Vector3::kZero, particleCamera_->GetViewOthographics());

	uiFrame_->Draw(uiCamera_->GetViewOthographics(), Pipeline::Normal, false);
	uiHp_->Draw(uiCamera_->GetViewOthographics(), Pipeline::Normal, false, false);

	uiBulletFrame_->Draw(uiCamera_->GetViewOthographics(), Pipeline::Normal, false);
	auto bullet = bullets_.begin();

	for (size_t i = 0; i < uiBullet_.size(); i++) {
		if (!(*bullet)->GetIsActive()) {
			uiBullet_[i]->Draw(uiCamera_->GetViewOthographics(), Pipeline::Normal, false, false);
		}
		bullet++;
	}
}

void Player::Debug([[maybe_unused]]const std::string& guiName)
{
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	model_->Debug(guiName.c_str());

	if (ImGui::TreeNode("ステータス")) {
		ImGui::DragFloat("最大速度", &maxSpeed_, 0.01f, 0.0f, std::numbers::pi_v<float> *2.0f);
		ImGui::DragFloat("速度加算", &addSpeed_, 0.01f, 0.0f, std::numbers::pi_v<float> * 2.0f);
		ImGui::DragFloat("速度減衰", &speedDecay_);
		ImGui::DragFloat("速度", &speed_);
		ImGui::DragFloat("攻撃", &attack_);
		ImGui::DragFloat3("オフセット", &offset_.x);
		ImGui::DragFloat3("エミッターポジション", &particle_->emitterPos.x);
		ImGui::Text("hp : %.0f", hp_);
		ImGui::TreePop();
	}
	ImGui::End();

	particle_->Debug("playerParticle");
#endif // _DEBUG
}

bool Player::Collision(const Enemy& enemy)
{
	const auto& enemyBullets = enemy.GetBullets();

	// もし無敵だったら
	if (isCollisioned_) {
		return false;
	}

	bool isCollision = false;

	for (const auto& i : enemyBullets) {
		if (i->GetIsActive()) {
			if (i->CollisionBullet(model_->pos, radius_)) {
				isCollision = true;
				isCollisioned_ = true;

				hp_ -= i->GetAttack();

				i->Unenable();
				return true;
			}
		}
	}

	return false;
}

void Player::Attack(const Enemy& enemy) {
	if (hp_ <= 0.0f) {
		return;
	}

	static KeyInput* const key = Input::GetInstance()->GetKey();
	static Gamepad* const gamepad = Input::GetInstance()->GetGamepad();

	if (currentBullet_ == bullets_.end()) {
		if (!bullets_.rbegin()->get()->GetIsActive()) {
			currentBullet_ = bullets_.begin();
		}
		else {
			return;
		}
	}

	if (key->Pushed(DIK_SPACE) || gamepad->Pushed(Gamepad::Button::A)) {
		float bulletIndex = static_cast<float>(std::distance(bullets_.begin(), currentBullet_));
		float bulletStateScale = 20.0f * (1.0f - (bulletIndex * 0.07f));

		currentBullet_->get()->SetStatus(
			model_->pos,
			(enemy.GetPos() - model_->pos).Normalize(),
			bulletStateScale,
			attack_ * bulletIndex,
			0x808080ff,
			0.5f
		);
		currentBullet_->get()->Enable();
		currentBullet_++;
	}
}

void Player::CreateBullets() {
	bullets_.resize(6u);

	for (auto& i : bullets_) {
		i.reset(new Bullet{});

		i->Initialize();
	}

	currentBullet_ = bullets_.begin();
}
