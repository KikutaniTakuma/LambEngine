#include "ButtonComp.h"

void ButtonComp::Init() {
	gamePad_ = Gamepad::GetInstance();
	keyInput_ = KeyInput::GetInstance();
	mouse_ = Mouse::GetInstance();
}

void ButtonComp::FirstUpdate() {
	isPushed_ = gamePad_->Pushed(gButton) or keyInput_->Pushed(keyButton) or mouse_->Pushed(mButton);
	isLongPushed_ = gamePad_->LongPush(gButton) or keyInput_->LongPush(keyButton) or mouse_->LongPush(mButton);
	isReleased_ = gamePad_->Released(gButton) or keyInput_->Released(keyButton) or mouse_->Released(mButton);
}

const Lamb::Flg& ButtonComp::Pushed()
{
	return isPushed_;
}

const Lamb::Flg& ButtonComp::LongPushed()
{
	return isLongPushed_;
}

const Lamb::Flg& ButtonComp::Released()
{
	return isReleased_;
}

void ButtonComp::Save(nlohmann::json& json) {
	SaveCompName(json);
	json["Gamepad"] = int32_t(gButton);
	json["key"] = int32_t(keyButton);
	json["Gamepad"] = int32_t(mButton);
}

void ButtonComp::Load(nlohmann::json& json)
{
	gButton = Gamepad::Button(json["Gamepad"].get<int32_t>());
	keyButton = uint8_t(json["key"].get<int32_t>());
	mButton = Mouse::Button(json["Gamepad"].get<int32_t>());
}
