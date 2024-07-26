#include "BurttonComp.h"

void BurttonComp::Init() {
	gamePad_ = Gamepad::GetInstance();
	keyInput_ = KeyInput::GetInstance();
	mouse_ = Mouse::GetInstance();
}

void BurttonComp::FirstUpdate() {
	isPushed_ = gamePad_->Pushed(gButton) or keyInput_->Pushed(keyButton) or mouse_->Pushed(mButton);
	isLongPushed_ = gamePad_->LongPush(gButton) or keyInput_->LongPush(keyButton) or mouse_->LongPush(mButton);
	isReleased_ = gamePad_->Released(gButton) or keyInput_->Released(keyButton) or mouse_->Released(mButton);
}

const Lamb::Flg& BurttonComp::Pushed()
{
	return isPushed_;
}

const Lamb::Flg& BurttonComp::LongPushed()
{
	return isLongPushed_;
}

const Lamb::Flg& BurttonComp::Released()
{
	return isReleased_;
}
