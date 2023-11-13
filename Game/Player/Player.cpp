#include "Player.h"
#include "Alfort/Input/Input.h"
#include "Alfort/Engine/FrameInfo/FrameInfo.h"
#include <numbers>
#include <cmath>
#include "externals/nlohmann/json.hpp"
#include "GlobalVariables/GlobalVariables.h"
#include "externals/imgui/imgui.h"

Player::Player(GlobalVariables* globalVariables):
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
	cmaeraRotateSpd_(std::numbers::pi_v<float>),
	cameraRotate_()
{
	model.push_back(std::make_unique<Model>());
	auto itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerBody.obj");

	model.push_back(std::make_unique<Model>());
	itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerHead.obj");
	(*itr)->SetParent(model.begin()->get());
	

	model.push_back(std::make_unique<Model>());
	itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerLeftArm.obj");
	(*itr)->SetParent(model.begin()->get());

	model.push_back(std::make_unique<Model>());
	itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerRightArm.obj");
	(*itr)->SetParent(model.begin()->get());

	const std::string groupName = "Player";
	const std::string groupName2 = "PlayerParts";
	globalVariables_ = globalVariables;
	globalVariables_->CreateGroup(groupName);
	globalVariables_->AddItem(groupName2, "Body Translation", model[0]->pos_);
	globalVariables_->AddItem(groupName2, "Head Translation", model[1]->pos_);
	globalVariables_->AddItem(groupName2, "LeftArm Translation", model[2]->pos_);
	globalVariables_->AddItem(groupName2, "RightArm Translation", model[3]->pos_);
	globalVariables_->AddItem(groupName, "Speed", spd);
	globalVariables_->AddItem(groupName, "freqSpd", freqSpd);
	globalVariables_->AddItem(groupName, "armFreqSpd", armFreqSpd);
	globalVariables_->AddItem(groupName, "attackSpd", attackSpd);
	globalVariables_->AddItem(groupName, "cmaeraRotateSpd_", cmaeraRotateSpd_);

	colliser_.collisionPos_.y = 1.85f;
	colliser_.scale_.y = 5.0f;
	colliser_.scale_.x = 2.09f;
	colliser_.scale_.z = 2.09f;

	pos_.y = 8.0f;
	pos_.x = 0.0f;
}

void Player::ApplyGlobalVariables() {
	const std::string groupName = "Player";
	const std::string groupName2 = "PlayerParts";
	model[0]->pos_ = globalVariables_->GetVector3Value(groupName2, "Body Translation");
	model[1]->pos_ = globalVariables_->GetVector3Value(groupName2, "Head Translation");
	model[2]->pos_ = globalVariables_->GetVector3Value(groupName2, "LeftArm Translation");
	model[3]->pos_ = globalVariables_->GetVector3Value(groupName2, "RightArm Translation");
	spd = globalVariables_->GetFloatValue(groupName, "Speed");
	freqSpd = globalVariables_->GetFloatValue(groupName, "freqSpd");
	armFreqSpd = globalVariables_->GetFloatValue(groupName, "armFreqSpd");
	attackSpd = globalVariables_->GetFloatValue(groupName, "attackSpd");
	cmaeraRotateSpd_ = globalVariables_->GetFloatValue(groupName, "cmaeraRotateSpd_");
}

void Player::Animation() {
	float deltaTime = FrameInfo::GetInstance()->GetDelta();
	freq += freqSpd * deltaTime;
	model[0]->pos_.y = std::sin(freq) + 2.5f;

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

		model[2]->rotate_.y = armFreq;
		model[3]->rotate_.y = armFreq;
		break;
	}
}

void Player::Move() {
	ApplyGlobalVariables();
	colliser_.UpdateCollision();
	moveVec_ = {};
	Animation();

	bool isMove = false;

	static Input* input = Input::GetInstance();

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
		moveVec_.x += spd * input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X);
		isMove = true;
	}
	if (input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) > 0.25f || input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) < -0.25f) {
		moveVec_.z += spd * input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y);
		isMove = true;
	}

	if (input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X) > 0.25f || input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X) < -0.25f) {
		cameraRotate_ += cmaeraRotateSpd_ * input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X) * FrameInfo::GetInstance()->GetDelta();

		isMove = true;
	}

	moveVec_ *= MakeMatrixRotateY(cameraRotate_);

	if (isMove) {
		Vector2 rotate;
		rotate.x = -moveVec_.x;
		rotate.y = moveVec_.z;

		model[0]->rotate_.y = rotate.GetRad() - (std::numbers::pi_v<float> *0.5f);
	}

	moveVec_.y = -15.0f;
	colliser_.collisionPos_ += moveVec_.Normalize() * spd * FrameInfo::GetInstance()->GetDelta();
}

void Player::Update() {
	if (moveVec_.y!=0.0f) {
		pos_.y += moveVec_.y * FrameInfo::GetInstance()->GetDelta();
		moveVec_.y = 0.0f;
	}
	pos_ += moveVec_.Normalize() * spd * FrameInfo::GetInstance()->GetDelta();
	colliser_.collisionPos_ = pos_;
	colliser_.collisionPos_.y += 1.85f;

	model[0]->pos_ = pos_;

	if (camera) {
		Vector3 offset = { 0.0f, 7.0f, -30.0f };
		camera->rotate.y = cameraRotate_;
		camera->rotate.x = 0.2f;
		offset *= MakeMatrixRotateY(cameraRotate_);
		camera->pos = model[0]->pos_ + offset;
		camera->Update();
	}
}

void Player::Draw() {
	for (auto& i : model) {
		i->Draw(camera->GetViewProjection(), camera->pos);
	}

	colliser_.DebugDraw(camera->GetViewProjection());
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