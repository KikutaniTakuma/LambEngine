#include "Player.h"
#include "Input/Input.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include <numbers>
#include <cmath>
#include "json.hpp"
#include "GlobalVariables/GlobalVariables.h"
#include "Utils/Random/Random.h"
#include "imgui.h"
#undef far
#undef near
#include "Utils/ScreenOut/ScreenOut.h"

const std::array<Player::ComboAttack, Player::kMaxComboNum_> Player::kComboAttacks_ = {
	Player::ComboAttack{
		std::chrono::milliseconds{200},

		float{std::numbers::pi_v<float> * 2.0f}
	},
	Player::ComboAttack{
		std::chrono::milliseconds{300},

		float{std::numbers::pi_v<float> * 4.0f}
	},
	Player::ComboAttack{
		std::chrono::milliseconds{1600},

		float{std::numbers::pi_v<float> * 8.0f}
	} 
};

void Player::Initialize(GlobalVariables* globalVariables) {
	const std::string groupName = "Player";
	const std::string groupName2 = "PlayerParts";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName2, "Body Translation", Vector3::zero);
	globalVariables->AddItem(groupName2, "Head Translation", Vector3::zero);
	globalVariables->AddItem(groupName2, "LeftArm Translation", Vector3::zero);
	globalVariables->AddItem(groupName2, "RightArm Translation", Vector3::zero);
	globalVariables->AddItem(groupName, "Speed", 10.0f);
	globalVariables->AddItem(groupName, "freqSpd", std::numbers::pi_v<float>);
	globalVariables->AddItem(groupName, "armFreqSpd", std::numbers::pi_v<float> / 2.0f);
	globalVariables->AddItem(groupName, "attackSpd", std::numbers::pi_v<float> / 2.0f);
	globalVariables->AddItem(groupName, "cmaeraRotateSpd_", std::numbers::pi_v<float>);
	globalVariables->AddItem(groupName, "dashCoolTime_", 500);
	globalVariables->AddItem(groupName, "dashScale_", 2.0f);
	globalVariables->AddItem(groupName, "jumpVelocity_", 9.8f);
	globalVariables->AddItem(groupName, "gravity_", -9.8f);
}

Player::Player(GlobalVariables* globalVariables) :
	spd(10.0f),
	moveVec_(),
	camera(nullptr),
	freqSpd(std::numbers::pi_v<float>),
	freq(0.0f),
	armFreqSpd(std::numbers::pi_v<float> / 2.0f),
	armFreq(0.0f),
	behavior(Behavior::Normal),
	attack(0.0f),
	attackSpd(std::numbers::pi_v<float> / 2.0f),
	dashStartTime_{},
	dashCoolTime_{ 500 },
	isDash_{ false },
	dashScale_{ 2.0f },
	jumpVelocity_{9.8f },
	gravity_{-9.8f }
{
	globalVariables_ = globalVariables;

	model_.push_back(std::make_unique<Model>());
	auto itr = model_.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerBody.obj");

	model_.push_back(std::make_unique<Model>());
	itr = model_.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerHead.obj");
	(*itr)->SetParent(model_.begin()->get());


	model_.push_back(std::make_unique<Model>());
	itr = model_.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerLeftArm.obj");
	(*itr)->SetParent(model_.begin()->get());

	model_.push_back(std::make_unique<Model>());
	itr = model_.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerRightArm.obj");
	(*itr)->SetParent(model_.begin()->get());

	weapon_ = std::make_unique<Model>();
	weapon_->LoadObj("AL_Resouce/Weapon/Hammer.obj");
	weapon_->SetParent(model_.begin()->get());
	weapon_->scale_ *= 0.5f;
	weapon_->pos_.y = 10.45f;
	weaponColliser_.scale_ = { 5.0f, 10.0f, 5.0f };

	collisionPos_.y = 1.85f;
	scale_.y = 5.0f;
	scale_.x = 2.09f;
	scale_.z = 2.09f;

	pos_.y = 16.0f;
	pos_.x = 0.0f;
}

void Player::ApplyGlobalVariables() {
	const std::string groupName = "Player";
	const std::string groupName2 = "PlayerParts";
	model_[0]->pos_ = globalVariables_->GetVector3Value(groupName2, "Body Translation");
	model_[1]->pos_ = globalVariables_->GetVector3Value(groupName2, "Head Translation");
	model_[2]->pos_ = globalVariables_->GetVector3Value(groupName2, "LeftArm Translation");
	model_[3]->pos_ = globalVariables_->GetVector3Value(groupName2, "RightArm Translation");
	spd = globalVariables_->GetFloatValue(groupName, "Speed");
	freqSpd = globalVariables_->GetFloatValue(groupName, "freqSpd");
	armFreqSpd = globalVariables_->GetFloatValue(groupName, "armFreqSpd");
	attackSpd = globalVariables_->GetFloatValue(groupName, "attackSpd");
	dashCoolTime_ = std::chrono::milliseconds{ globalVariables_->GetIntValue(groupName, "dashCoolTime_") };
	dashScale_ = globalVariables_->GetFloatValue(groupName, "dashScale_");
	jumpVelocity_ = globalVariables_->GetFloatValue(groupName, "jumpVelocity_");
	gravity_ = globalVariables_->GetFloatValue(groupName, "gravity_");
}

void Player::Animation() {
	float deltaTime = FrameInfo::GetInstance()->GetDelta();
	std::chrono::steady_clock::time_point nowTime = FrameInfo::GetInstance()->GetThisFrameTime();
	freq += freqSpd * deltaTime;
	model_[0]->pos_.y = std::sin(freq) + 2.5f;

	Vector3 trembling;
	float tremblingStrength = 1.0f;

	if (freq > (std::numbers::pi_v<float> *2.0f)) {
		freq = 0.0f;
	}

	switch (behavior)
	{
	case Player::Behavior::Normal:
	default:
		armFreq += armFreqSpd * deltaTime;

		if (armFreq > std::numbers::pi_v<float> *2.0f) {
			armFreq = 0.0f;
		}

		model_[2]->rotate_.y = armFreq;
		model_[3]->rotate_.y = armFreq;
		isWeaopnCollsion_ = false;
		break;
	case Player::Behavior::Attack:
		if (kComboAttacks_[workAttack_.currentComboAttack_].chargeTime_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - attackStartTime_) ) {
			armFreq += attackSpd * deltaTime;
		}else if(workAttack_.currentComboAttack_ == kMaxComboNum_ -1) {
			trembling = Lamb::Random(-Vector3::identity * tremblingStrength, Vector3::identity * tremblingStrength);
		}
		else {
			trembling = Vector3::zero;
		}

		if (armFreq > (std::numbers::pi_v<float> *0.5f)) {
			armFreq = 0.0f;
			if (workAttack_.isNext_) {
				behavior = Behavior::Attack;
				attackStartTime_ = nowTime;
				workAttack_.isNext_ = false;
				armFreq = 0.0f;

				model_[2]->rotate_.y = 0.0f;
				model_[3]->rotate_.y = 0.0f;
				workAttack_.currentComboAttack_++;
				if (kMaxComboNum_ <= workAttack_.currentComboAttack_) {
					workAttack_.isNext_ = false;
					workAttack_.currentComboAttack_ = kMaxComboNum_ - 1;
				}
				attackSpd = kComboAttacks_[workAttack_.currentComboAttack_].swingSpeed_;
				isWeaopnCollsion_ = false;
			}
			else {
				if (kMaxComboNum_ - 1 == workAttack_.currentComboAttack_) {
					workAttack_.isNext_ = false;
					workAttack_.currentComboAttack_ = 0;
				}
				behavior = Behavior::Normal;
			}
			model_[2]->rotate_.x = 0.0f;
			model_[3]->rotate_.x = 0.0f;
			break;
		}
		if (std::chrono::milliseconds{300} < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - attackStartTime_)) {
			isWeaopnCollsion_ = true;
		}
		else {
			isWeaopnCollsion_ = false;
		}

		model_[2]->rotate_.x = armFreq + std::numbers::pi_v<float>;
		model_[3]->rotate_.x = armFreq + std::numbers::pi_v<float>;

		weapon_->rotate_.x = armFreq;
		weapon_->Debug("weapon_");
		weapon_->Update();
		weapon_->worldMat_ = MakeMatrixAffin(weapon_->scale_, Vector3::zero, weapon_->pos_ + trembling);
		weapon_->worldMat_ *= MakeMatrixRotate(weapon_->rotate_);
		break;
	}
}

void Player::Move(const Mat4x4& cameraRoatate) {
	isDash_.Update();
	ApplyGlobalVariables();
	UpdateCollision();
	weaponColliser_.UpdateCollision();
	Animation();

	bool isMove = false;
	moveVec_.x = 0.0f;
	moveVec_.z = 0.0f;
	static Input* const input = Input::GetInstance();
	auto nowTime = FrameInfo::GetInstance()->GetThisFrameTime();

	if (!isDash_) {
		if (input->GetKey()->LongPush(DIK_W)) {
			moveVec_.z += spd;
			isMove = true;
		}
		if (input->GetKey()->LongPush(DIK_A)) {
			moveVec_.x -= spd;
			isMove = true;
		}
		if (input->GetKey()->LongPush(DIK_S)) {
			moveVec_.z -= spd;
			isMove = true;
		}
		if (input->GetKey()->LongPush(DIK_D)) {
			moveVec_.x += spd;
			isMove = true;
		}

		if (input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) > 0.25f || input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) < -0.25f) {
			moveVec_.x = 0.0f;
			moveVec_.x += spd * input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X);
			isMove = true;
		}
		if (input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) > 0.25f || input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) < -0.25f) {
			moveVec_.z = 0.0f;
			moveVec_.z += spd * input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y);
			isMove = true;
		}

		if (moveVec_.y == 0.0f && (input->GetKey()->LongPush(DIK_SPACE) ||
			input->GetGamepad()->Pushed(Gamepad::Button::A))
			)
		{
			moveVec_.y += jumpVelocity_;
		}
	}

	moveVec_ *= cameraRoatate;

	if (!isDash_) {
		if (input->GetKey()->LongPush(DIK_LSHIFT) ||
			input->GetGamepad()->Pushed(Gamepad::Button::B)) {
			isDash_ = true;
			dashStartTime_ = nowTime;

		}
	}

	if (isDash_.OnEnter()) {
		spd *= dashScale_;
		moveVec_ = Vector3::zIdy;
		if (preMoveVec_.Normalize() == -Vector3::zIdy) {
			moveVec_ *= DirectionToDirection(-Vector3::zIdy, Vector3{ preMoveVec_.x,0.0f, preMoveVec_.z }.Normalize());
		}
		else {
			moveVec_ *= DirectionToDirection(Vector3::zIdy, Vector3{ preMoveVec_.x,0.0f, preMoveVec_.z }.Normalize());
		}
	}


	if (isDash_) {
		if (dashCoolTime_ <= std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - dashStartTime_)) {
			isDash_ = false;
			spd /= dashScale_;
		}
	}

	float deltaTime = FrameInfo::GetInstance()->GetDelta();
	moveVec_.y += gravity_ * deltaTime;

	collisionPos_.x += moveVec_.x * spd * deltaTime;
	collisionPos_.y += moveVec_.y;
	collisionPos_.z += moveVec_.z * spd * deltaTime;
}

void Player::Update() {
	//ApplyGlobalVariables();
	auto nowTime = FrameInfo::GetInstance()->GetThisFrameTime();

	if (moveVec_.y != 0.0f) {
		pos_.y += moveVec_.y * FrameInfo::GetInstance()->GetDelta();
		//moveVec_.y = 0.0f;
	}
	pos_ += moveVec_.Normalize() * spd * FrameInfo::GetInstance()->GetDelta();
	collisionPos_ = pos_;
	collisionPos_.y += 1.85f;

	model_[0]->pos_ = pos_;

	Lamb::screenout << "IsNext" << workAttack_.isNext_ << Lamb::endline 
		<< "nowCombo" << workAttack_.currentComboAttack_ << Lamb::endline
		<< "isWeaponColision" << isWeaopnCollsion_ << Lamb::endline;

	if ((KeyInput::GetInstance()->Pushed(DIK_RETURN) || Gamepad::GetInstance()->Pushed(Gamepad::Button::X)) && workAttack_.currentComboAttack_ < kMaxComboNum_ - 1) {
		if (behavior == Behavior::Attack && !workAttack_.isNext_ ) {
			if (kMaxComboNum_ <= workAttack_.currentComboAttack_) {
				workAttack_.isNext_ = false;
				workAttack_.currentComboAttack_ = kMaxComboNum_-1;
			}
			else {
				workAttack_.isNext_ = true;
			}
		}
		else {
			workAttack_.isNext_ = false;
			workAttack_.currentComboAttack_ = 0;
			behavior = Behavior::Attack;
			attackStartTime_ = nowTime;

			armFreq = 0.0f;

			model_[2]->rotate_.y = 0.0f;
			model_[3]->rotate_.y = 0.0f;
		}
	}

	if (moveVec_ != Vector3::zero && !isDash_) {
		preMoveVec_ = moveVec_;
	}

	for (auto& i : model_) {
		i->Update();
	}
	if (Vector3{ preMoveVec_.x,0.0f, preMoveVec_.z } != Vector3::zero) {
		if (Vector3{ preMoveVec_.x,0.0f, preMoveVec_.z }.Normalize() == -Vector3::zIdy) {
			model_[0]->worldMat_ = MakeMatrixScalar(model_[0]->scale_) * DirectionToDirection(-Vector3::zIdy, Vector3{ preMoveVec_.x,0.0f, preMoveVec_.z }.Normalize()) * MakeMatrixRotateY(std::numbers::pi_v<float>) * MakeMatrixTranslate(model_[0]->pos_);
		}
		else {
			model_[0]->worldMat_ = MakeMatrixScalar(model_[0]->scale_) * DirectionToDirection(Vector3::zIdy, Vector3{ preMoveVec_ .x,0.0f, preMoveVec_ .z}.Normalize()) * MakeMatrixTranslate(model_[0]->pos_);
		}
	}
	/*weapon_->Debug("weapon_");
	weapon_->Update();
	if (behavior == Behavior::Attack) {
		weapon_->worldMat_.Affin(weapon_->scale_, Vector3::zero, weapon_->pos_);
		weapon_->worldMat_ *= MakeMatrixRotate(weapon_->rotate_);
	}*/
	weaponColliser_.collisionPos_ = weapon_->GetPos();
}

void Player::Draw() {
	for (auto& i : model_) {
		i->Draw(camera->GetViewProjection(), camera->pos);
	}

	if (behavior == Behavior::Attack) {
		weapon_->Draw(camera->GetViewProjection(), camera->pos);
		weaponColliser_.DebugDraw(camera->GetViewProjection());
	}

	DebugDraw(camera->GetViewProjection());
}

void Player::Debug() {
	/*model[0]->Debug("Player");
	ImGui::Begin("Player");
	ImGui::DragFloat("spd", &spd);
	if (ImGui::TreeNode("Freq")) {
		ImGui::DragFloat("Freq", &freq);
		ImGui::DragFloat("FreqSpd", &freqSpd);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("armFreq")) {
		ImGui::DragFloat("armFreq", &armFreq);
		ImGui::DragFloat("armFreqSpd", &armFreqSpd);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Attack")) {
		ImGui::DragFloat("attack", &attack);
		ImGui::DragFloat("attackSpd", &attackSpd);
		ImGui::TreePop();
	}
	ImGui::End();*/
}