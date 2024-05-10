#include "Player.h"
#include "Camera/Camera.h"
#include "Input/Gamepad/Gamepad.h"
#include "Input/KeyInput/KeyInput.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Engine/Graphics/AnimationManager/AnimationManager.h"

void Player::Init(const Transform& transform)
{
	std::string fileName = "./Resources/Common/Player/player.gltf";
	std::string punchFileName = "./Resources/Common/Player/playerPunch.gltf";
	std::string walkFileName = "./Resources/Common/Player/walk.gltf";
	std::string floatingFileName = "./Resources/Common/Player/playerFloating.gltf";
	Lamb::SafePtr animationManager = AnimationManager::GetInstance();
	animationManager->LoadAnimations(walkFileName);
	animationManager->LoadAnimations(fileName);
	animationManager->LoadAnimations(punchFileName);
	animationManager->LoadAnimations(floatingFileName);

	model_ = std::make_unique<AnimationModel>(fileName);
	model_->GetAnimator().SetLoopAnimation(true);
	model_->GetAnimator().Start();
	waitAnimatons_ = animationManager->GetAnimations(fileName);
	punchAnimatons_ = animationManager->GetAnimations(punchFileName);
	walkAnimatons_ = animationManager->GetAnimations(walkFileName);
	floatingAnimatons_ = animationManager->GetAnimations(floatingFileName);
	transform_ = transform;
	transform_.scale *= 2.0f;

	gravity_ = 9.80665f * 0.2f;

	jump_ = 30.0f;

	speed_ = 20.0f;

	obb_ = MakeObb();
}

void Player::Update() {
	transform_.Debug("player");
	model_->Update();

	if (isPunch_ and model_->GetAnimator().GetIsActive().OnExit()) {
		isPunch_ = false;
		model_->GetAnimator().SetAnimations(waitAnimatons_);
		model_->GetAnimator().SetLoopAnimation(true);
		/*model_->GetAnimator().SetIsFullAnimation(false);
		model_->GetAnimator().SetAnimationIndex(0);*/
		model_->GetAnimator().Start();
	}

	Move();
	Jump();
	Falling();
	if (not isJump_) {
		Punch();
	}


	transform_.translate.x += direction_.x * speed_ * Lamb::DeltaTime();
	transform_.translate.z += direction_.y * speed_ * Lamb::DeltaTime();

	transform_.translate.y += jumpSpeed_ * Lamb::DeltaTime();

	transform_.rotate = Vector3::QuaternionToEuler(Quaternion::DirectionToDirection(Vector3::kZIdentity, Vector3(direction_.x, 0.0f, direction_.y)));

	obb_->transform.translate = transform_.translate;
	obb_->transform.scale = transform_.scale;
	obb_->Update();
}
void Player::AfterCollisionUpdate([[maybe_unused]]const Vector3& pushVector) {
	obb_->transform.translate += pushVector;
	transform_.translate = obb_->transform.translate;
	obb_->Update();
}

void Player::Draw(const Camera& camera) {
	model_->Draw(
		Mat4x4::MakeAffin(Vector3::kIdentity, Vector3::kZero, Vector3::kYIdentity * -0.5f) * transform_.GetMatrix(),
		camera.GetViewProjection(),
		0xffffffff,
		BlendType::kNormal,
		true
	);

	obb_->Draw(camera.GetViewProjection());
}

void Player::Punch() {
	Lamb::SafePtr gamepad = Gamepad::GetInstance();
	if (not isPunch_ and gamepad->GetButton(Gamepad::Button::B)) {
		model_->GetAnimator().Stop();
		model_->GetAnimator().SetAnimations(punchAnimatons_);
		model_->GetAnimator().SetLoopAnimation(false);
		//model_->GetAnimator().SetIsFullAnimation(true);
		model_->GetAnimator().Start();
		isPunch_ = true;
	}
}

void Player::Move() {
	if (not isPunch_) {
		Lamb::SafePtr gamepad = Gamepad::GetInstance();
		Vector2&& stick = gamepad->GetStick(Gamepad::Stick::LEFT);

		if(isJump_){
			model_->GetAnimator().SetAnimations(floatingAnimatons_);
			model_->GetAnimator().SetAnimationSpeed(2.0f);
		}
		else if (stick == Vector2::kZero) {
			model_->GetAnimator().SetAnimations(waitAnimatons_);
		}
		else {
			model_->GetAnimator().SetAnimations(walkAnimatons_);
		}

		direction_ = stick;
	}
	else {
		direction_ = Vector2::kZero;
	}
}

void Player::Jump() {
	Lamb::SafePtr gamepad = Gamepad::GetInstance();
	if (not isJump_ and gamepad->GetButton(Gamepad::Button::A)) {
		isJump_ = true;

		jumpSpeed_ = jump_;
		jumpTime_ = 0.0f;
	}
}

void Player::Falling() {
	if (isJump_) {
		jumpSpeed_ += -gravity_ * jumpTime_;
		jumpTime_ += Lamb::DeltaTime();
	}
}

void Player::Landing(bool isCollision) {
	if (transform_.translate.y <= 0.0f) {
		transform_.translate.y = 0.0f;
		isJump_ = false;
		JumpReset();
	}
	else if(isCollision){
		isJump_ = false;
		JumpReset();
	}
	else if(not isJump_ and not isCollision){
		isJump_ = true;

		jumpSpeed_ = 0.0f;
		jumpTime_ = 0.0f;
	}
}

void Player::JumpReset()
{
	jumpSpeed_ = 0.0f;
	jumpTime_ = 0.0f;
	model_->GetAnimator().SetAnimationSpeed(1.0f);
}
