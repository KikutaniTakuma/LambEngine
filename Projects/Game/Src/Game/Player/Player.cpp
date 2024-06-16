#include "Player.h"
#include "Camera/Camera.h"
#include "Input/Gamepad/Gamepad.h"
#include "Input/KeyInput/KeyInput.h"
#include "Utils/EngineInfo.h"
#include "Engine/Graphics/AnimationManager/AnimationManager.h"

void Player::Init(const Transform& transformInput)
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
	transform = transformInput;
	transform.scale *= 2.0f;

	gravity_ = 9.80665f * 0.2f;

	jump_ = 30.0f;

	speed_ = 20.0f;

	obb_ = MakeObb();
}

void Player::Update(const Vector3& cameraRotate) {
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

	Vector3 directionTmp = Vector3(direction_.x, 0.0f, direction_.y) * Quaternion::MakeRotateYAxis(cameraRotate.y);

	directionTmp = directionTmp.Normalize();
	transform.translate.x += directionTmp.x * speed_ * Lamb::DeltaTime();
	transform.translate.z += directionTmp.z * speed_ * Lamb::DeltaTime();

	transform.translate.y += jumpSpeed_ * Lamb::DeltaTime();

	Vector3 toDir = Vector3(playerDirection_.x, 0.0f, playerDirection_.y).Normalize();
	toDir.x *= (direction_.y < 0.0f ? -1.0f : 1.0f);
	const Vector3& fromDir = Vector3::kZIdentity;

	transform.rotate = Quaternion::DirectionToDirection(fromDir, toDir).ToEuler();
	transform.rotate.y += cameraRotate.y;

	obb_->transform.translate = transform.translate;
	obb_->transform.scale = transform.scale;
	obb_->Update();
}
void Player::AfterCollisionUpdate([[maybe_unused]]const Vector3& pushVector) {
	obb_->transform.translate += pushVector;
	transform.translate = obb_->transform.translate;
	obb_->Update();
}

void Player::Draw(const Camera& camera) {
	model_->Draw(
		Mat4x4::MakeTranslate(Vector3::kYIdentity * -0.5f) * transform.GetMatrix(),
		camera.GetViewProjection(),
		0xffffffff,
		BlendType::kNormal,
		true
	);

	obb_->Draw(camera.GetViewProjection());
}

void Player::Punch() {
	Lamb::SafePtr gamepad = Gamepad::GetInstance();
	Lamb::SafePtr key = KeyInput::GetInstance();
	if (not isPunch_ and (gamepad->Pushed(Gamepad::Button::B) or key->Pushed(DIK_SPACE))) {
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
		if (direction_ != Vector2::kZero) {
			playerDirection_ = direction_.Normalize();
		}
	}
	else {
		direction_ = Vector2::kZero;
	}
}

void Player::Jump() {
	Lamb::SafePtr gamepad = Gamepad::GetInstance();
	if (not isJump_ and gamepad->Pushed(Gamepad::Button::A)) {
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
	if (transform.translate.y <= 0.0f) {
		transform.translate.y = 0.0f;
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
