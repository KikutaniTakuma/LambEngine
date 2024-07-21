#include "FollowCamera.h"

#include <algorithm>
#include <cmath>

#include "Input/Input.h"
#include "Math/Mat4x4.h"
#include "Utils/EngineInfo/EngineInfo.h"


FollowCamera::FollowCamera() {
	target_ = new Target();
	name_ = "FollowCamera";
	globalVariables_ = std::make_unique<GlobalVariables>(name_);
}

void FollowCamera::Initialize() {
	SetGlobalVariable();
}

void FollowCamera::Update() {
	if (target_) {
		interTarget_.x = std::lerp(interTarget_.x, target_->position.x, 1.0f);
		interTarget_.y = std::lerp(interTarget_.y, target_->position.y, 1.0f);
		interTarget_.z = std::lerp(interTarget_.z, target_->position.z, 1.0f);

		// ゲームパットのカメラ処理
		GamePad();
		// キーボードのカメラ処理
		Keyboard();
		// マウス
		Mouse();
		//// 回転を更新
		UpdateRotate();
		// 
		camera_->Update();
	}
#ifdef _DEBUG
	ApplyGlobalVariable();
	globalVariables_->Update();
#endif // _DEBUG
}

void FollowCamera::UpdateRotate() {
	static const double PI = 3.14159265359;
	destinationAngle_.x = std::clamp(destinationAngle_.x, float(0.0f * PI / 180.0f), float(80.0f * PI / 180.0f));
	Vector3 cameraMoveDelay = vector3Parameter_.at(Vector3Parameter::kCameraMoveDeray);
	camera_->rotate.x = std::lerp(camera_->rotate.x, destinationAngle_.x, cameraMoveDelay.x);
	camera_->rotate.y = std::lerp(camera_->rotate.y, destinationAngle_.y, cameraMoveDelay.y);
	camera_->rotate.z = std::lerp(camera_->rotate.z, destinationAngle_.z, cameraMoveDelay.z);
	// 回転行列生成
	Mat4x4 rotate = Mat4x4::MakeRotateX(camera_->rotate.x) * Mat4x4::MakeRotateY(camera_->rotate.y);
	// オフセットをカメラの回転に合わせて回転させる
	Vector3 offset = vector3Parameter_.at(Vector3Parameter::kOffset);
	offset *= rotate;
	// 座標をコピーしてずらす 
	camera_->pos = interTarget_ + offset;
}

void FollowCamera::GamePad() {
	auto gamepad = Input::GetInstance()->GetGamepad();
	Vector3 rotate = { gamepad->GetStick(Gamepad::Stick::RIGHT_Y) ,gamepad->GetStick(Gamepad::Stick::RIGHT_X),0.0f };
	if (rotate.Length() != 0) {
		rotate.Normalize();
		rotate.x *= -vector2Parameter_.at(Vector2Parameter::kCamaraSpeed).x;
		rotate.y *= vector2Parameter_.at(Vector2Parameter::kCamaraSpeed).y;
		destinationAngle_ += rotate;
	}
}

void FollowCamera::Keyboard() {
	auto key = Input::GetInstance()->GetKey();
	Vector3 vector{};
	if (key->LongPush(DIK_RIGHTARROW)) {
		vector.y = 1.0f;
	}
	if (key->LongPush(DIK_LEFTARROW)) {
		vector.y = -1.0f;
	}
	if (key->LongPush(DIK_UPARROW)) {
		vector.x = 1.0f;
	}
	if (key->LongPush(DIK_DOWNARROW)) {
		vector.x = -1.0f;
	}
	if (vector.Length() != 0.0f) {
		vector.Normalize();
		destinationAngle_.x += vector.x * vector2Parameter_.at(Vector2Parameter::kCamaraSpeed).x;
		destinationAngle_.y += vector.y * vector2Parameter_.at(Vector2Parameter::kCamaraSpeed).y;
	}
}

void FollowCamera::Mouse() {
	auto mouse = Input::GetInstance()->GetMouse();
	if(mouse->Pushed(Mouse::Button::Left)){
		prePos_ = mouse->GetPos();
	}
	if (mouse->LongPush(Mouse::Button::Left)) {
		isUsedCamera_ = true;
		destinationAngle_.x -= (mouse->GetVelocity().y * 0.05f) * vector2Parameter_.at(Vector2Parameter::kCamaraSpeed).x;
		destinationAngle_.y += (mouse->GetVelocity().x * 0.05f) * vector2Parameter_.at(Vector2Parameter::kCamaraSpeed).y;
	}
	else {
		isUsedCamera_ = false;
	}
}

float FollowCamera::LerpShortAngle(float a, float b, float t) {
	static const double PI = 3.14159265359;
	// 角度差分を求める
	float diff = b - a;
	// 角度[-2PI,+2PI]に補正
	diff = std::fmod(diff, 360.0f * static_cast<float>(PI) / 180.0f);
	// 角度[-PI,+PI]に補正
	if (diff > 180.0f * static_cast<float>(PI) / 180.0f) {
		diff -= 360.0f * static_cast<float>(PI) / 180.0f;
	}
	else if (diff < -180.0f * static_cast<float>(PI) / 180.0f) {
		diff += 360.0f * static_cast<float>(PI) / 180.0f;
	}
	return std::lerp(a, a + diff, t);
}

void FollowCamera::SetGlobalVariable() {
	globalVariables_->CreateGroup(name_);
	/*for (int i = 0; i < FloatFollowCameraParameter::kFloatFollowCameraParameterCount; i++) {
		globalVariables_->AddItem(name_, floatParameterItemNames_.at(i), floatParameter_.at(i));
	}*/

	for (int i = 0; i < Vector2Parameter::kVector2ParameterCount; i++) {
		globalVariables_->AddItem(name_, vector2ParameterItemNames_.at(i), vector2Parameter_.at(i));
	}

	for (int i = 0; i < Vector3Parameter::kVector3ParameterCount; i++) {
		globalVariables_->AddItem(name_, vector3ParameterItemNames_.at(i), vector3Parameter_.at(i));
	}

	globalVariables_->LoadFile();

	ApplyGlobalVariable();
}

void FollowCamera::ApplyGlobalVariable() {

	/*for (int i = 0; i < FloatFollowCameraParameter::kFloatFollowCameraParameterCount; i++) {
		floatParameter_.at(i) = globalVariables_->GetFloatValue(name_, floatParameterItemNames_.at(i));
	}*/

	for (int i = 0; i < Vector2Parameter::kVector2ParameterCount; i++) {
		vector2Parameter_.at(i) = globalVariables_->GetVector2Value(name_, vector2ParameterItemNames_.at(i));
	}

	for (int i = 0; i < Vector3Parameter::kVector3ParameterCount; i++) {
		vector3Parameter_.at(i) = globalVariables_->GetVector3Value(name_, vector3ParameterItemNames_.at(i));
	}
}
