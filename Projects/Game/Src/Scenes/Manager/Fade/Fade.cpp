#include "Fade.h"
#include "Utils/EngineInfo.h"
#include "Engine/Graphics/RenderingManager/RenderingManager.h"
#include "Utils/HSV.h"
#include "imgui.h"

#include <climits>

Fade::Fade() :
	isInStart_(false),
	isOutStart_(false),
	color_(Vector4::kIdentity),
	fadeTime_(0.75f)
{
}

void Fade::OutStart() {
	if (!isInStart_ && !isOutStart_) {
		isOutStart_ = true;
		ease_.Start(
			false,
			fadeTime_
		);
	}
}
void Fade::InStart() {
	if (!isInStart_ && !isOutStart_) {
		isInStart_ = true;
		ease_.Start(
			false,
			fadeTime_
		);
	}
}
bool Fade::OutEnd() const {
	return isOutStart_.OnExit();
}
bool Fade::OutStay() const {
	return isOutStart_.OnStay();
}

bool Fade::InEnd() const {
	return isInStart_.OnExit();
}
bool Fade::InStay() const {
	return isInStart_.OnStay();
}

bool Fade::IsActive() const
{
	return isInStart_ || isOutStart_;
}

void Fade::Update() {
	if (isInStart_) {
		color_ = ColorLerp(Vector4::kZero + Vector4::kWIdentity, Vector4::kIdentity, ease_.GetT());
	}
	else if (isOutStart_) {
		color_ = ColorLerp(Vector4::kIdentity, Vector4::kZero + Vector4::kWIdentity, ease_.GetT());
	}

	ease_.Update();

	if (ease_.ActiveExit()) {
		isInStart_ = false;
		isOutStart_ = false;
	}
	if (isInStart_ || isOutStart_) {
		RenderingManager::GetInstance()->SetColor(color_);
	}
	else {
		RenderingManager::GetInstance()->SetColor(Vector4::kIdentity);
	}
}