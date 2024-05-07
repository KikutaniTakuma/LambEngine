#include "Player.h"
#include "Camera/Camera.h"
#include "Input/Gamepad/Gamepad.h"
#include "Input/KeyInput/KeyInput.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Drawers/DrawerManager.h"

void Player::Init(const Transform& transform)
{
	std::string fileName = "./Resources/Common/Player/Player.gltf";
	std::string punchFileName = "./Resources/Common/Player/PlayerPunch.gltf";
	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();


	drawerManager->LoadModel(fileName);
	model_ = drawerManager->GetModel(fileName);
	waitAnimator_ = std::make_unique<Animator>();
	waitAnimator_->Load(fileName);
	waitAnimator_->SetIsFullAnimation(false);
	waitAnimator_->SetLoopAnimation(true);
	waitAnimator_->Start();
	punchAnimator_ = std::make_unique<Animator>();
	punchAnimator_->Load(punchFileName);
	punchAnimator_->SetIsFullAnimation(false);
	punchAnimator_->SetLoopAnimation(false);

	transform_ = transform;
	transform_.rotate = { 1.57f, 0.0f, 1.57f };

	gravity_ = 9.80665f;

	jump_ = 30.0f;

	speed_ = 20.0f;

	obb_ = MakeObb();
}

void Player::Update() {
	transform_.Debug("player");


	punchAnimator_->Update(model_->GetNode().name);
	waitAnimator_->Update(model_->GetNode().name);
	if (punchAnimator_->GetIsActive().OnExit()) {
		waitAnimator_->Start();
		punchAnimator_->Stop();
		isPunch_ = false;
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


	obb_->transform = transform_;
	obb_->transform.scale *= 2.0f;
	obb_->transform.rotate -= (Vector3::kXIdentity + Vector3::kZIdentity) * 1.57f;
	obb_->Update();
}
void Player::AfterCollisionUpdate([[maybe_unused]]const Vector3& pushVector) {
	obb_->transform.translate += pushVector;
	transform_.translate = obb_->transform.translate;
	obb_->Update();
}

void Player::Draw(const Camera& camera) {
	const Mat4x4& localMatrix = punchAnimator_->GetIsActive() ? punchAnimator_->GetLocalMat4x4() : waitAnimator_->GetLocalMat4x4();

	model_->Draw(
		localMatrix * transform_.GetMatrix(),
		camera.GetViewProjection(),
		0xffffffff,
		BlendType::kNormal
	);

	obb_->Draw(camera.GetViewProjection());
}

void Player::Punch() {
	Lamb::SafePtr gamepad = Gamepad::GetInstance();
	if (not isPunch_ and gamepad->GetButton(Gamepad::Button::B)) {
		punchAnimator_->Start();
		isPunch_ = true;
	}
}

void Player::Move() {
	Lamb::SafePtr gamepad = Gamepad::GetInstance();
	Vector2&& stick = gamepad->GetStick(Gamepad::Stick::LEFT);

	direction_ = Vector2::kIdentity;

	direction_ = stick;
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
}
