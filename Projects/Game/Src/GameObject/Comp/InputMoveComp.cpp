#include "InputMoveComp.h"
#include "Input/Input.h"

void InputMoveComp::FirstUpdate()
{
	Lamb::SafePtr gamepad = Input::GetInstance()->GetGamepad();
	Lamb::SafePtr key = Input::GetInstance()->GetKey();
	Lamb::SafePtr mouse = Input::GetInstance()->GetMouse();

	if (gamepad->PushAnyKey()) {
		isPad_ = true;
	}
	else if (key->PushAnyKey() or mouse->PushAnyKey()) {
		isPad_ = false;
	}

	if (isPad_) {
		direction_ = gamepad->GetStick(Gamepad::Stick::LEFT);
	}
	else {
		if (key->GetKey(DIK_W)) {
			direction_.z += 1.0f;
		}
		if (key->GetKey(DIK_A)) {
			direction_.x -= 1.0f;
		}
		if (key->GetKey(DIK_S)) {
			direction_.z -= 1.0f;
		}
		if (key->GetKey(DIK_D)) {
			direction_.x += 1.0f;
		}

		direction_ = direction_.Normalize();
	}

	moveVector_ = direction_ * speed_;
}