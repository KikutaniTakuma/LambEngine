#include "InputJump.h"
#include "Input/Input.h"

void InputJump::FirstUpdate()
{
	Lamb::SafePtr gamepad = Input::GetInstance()->GetGamepad();
	Lamb::SafePtr key = Input::GetInstance()->GetKey();

	if (gamepad->Pushed(Gamepad::Button::A) or key->Pushed(DIK_SPACE)) {
		this->strength_ = speed_;
	}
	else {
		this->strength_ = 0.0_f32;
	}
}
