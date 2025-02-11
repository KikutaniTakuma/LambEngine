/// ============================
/// ==  KeyInputクラスの定義  ==
/// ============================


#include "KeyInput.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include <cassert>
#include "Utils/ExecutionLog.h"
#include "Error/Error.h"

#ifdef USE_DEBUG_CODE
#include "imgui.h"
#endif // USE_DEBUG_CODE


void KeyInput::Input() {
	if (!initalizeSucceeded_) {
		return;
	}

	std::copy(key_.begin(), key_.end(), preKey_.begin());
	
	// キーボード情報取得開始
	keyBoard_->Acquire();

	// キー入力
	key_ = { 0 };
	keyBoard_->GetDeviceState(DWORD(key_.size()), key_.data());
}

void KeyInput::InputReset() {
	key_ = { 0 };
	std::copy(key_.begin(), key_.end(), preKey_.begin());
}

bool KeyInput::Pushed(uint8_t keyType) {
	if (!initalizeSucceeded_) {
		return false;
	}
#ifdef USE_DEBUG_CODE
	if (ImGui::GetIO().WantCaptureMouse or ImGui::IsAnyItemHovered()) {
		return false;
	}
#endif // USE_DEBUG_CODE
	return (key_[keyType] & 0x80) && !(preKey_[keyType] & 0x80);
}
bool KeyInput::LongPush(uint8_t keyType) {
	if (!initalizeSucceeded_) {
		return false;
	}
	return (key_[keyType] & 0x80) && (preKey_[keyType] & 0x80);
}
bool KeyInput::Released(uint8_t keyType) {
	if (!initalizeSucceeded_) {
		return false;
	}
#ifdef USE_DEBUG_CODE
	if (ImGui::GetIO().WantCaptureMouse or ImGui::IsAnyItemHovered()) {
		return false;
	}
#endif // USE_DEBUG_CODE
	return !(key_[keyType] & 0x80) && (preKey_[keyType] & 0x80);
}

bool KeyInput::PushAnyKey() {
#ifdef USE_DEBUG_CODE
	if (ImGui::GetIO().WantCaptureMouse or ImGui::IsAnyItemHovered()) {
		return false;
	}
#endif // USE_DEBUG_CODE
	return key_ != preKey_;
}

KeyInput* KeyInput::pInstance_ = nullptr;

void KeyInput::Initialize(IDirectInput8* input) {
	assert(input);
	pInstance_ = new KeyInput(input);
}
void KeyInput::Finalize() {
	delete pInstance_;
	pInstance_ = nullptr;
}

KeyInput::KeyInput(IDirectInput8* input):
	keyBoard_(),
	key_{0},
	preKey_{0},
	initalizeSucceeded_(false)
{
	
	HRESULT hr = input->CreateDevice(GUID_SysKeyboard, keyBoard_.GetAddressOf(), NULL);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<KeyInput>("CreateDevice failed",ErrorPlace);
	}

	hr = keyBoard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<KeyInput>("SetDataFormat failed",ErrorPlace);
	}

	hr = keyBoard_->SetCooperativeLevel(WindowFactory::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		throw Lamb::Error::Code<KeyInput>("SetDataFormat failed",ErrorPlace);
	}

	initalizeSucceeded_ = true;

	Lamb::AddLog("Initialize KeyInput succeeded");
}

KeyInput::~KeyInput() {
	keyBoard_->Unacquire();
}