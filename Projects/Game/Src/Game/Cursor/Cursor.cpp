#include "Cursor.h"

#include <numbers>

#include "imgui.h"
#include "Input/Input.h"

#include "Drawers/DrawerManager.h"
#include "Math/MathCommon.h"

void Cursor::Initialize() {
	DrawerManager::GetInstance()->LoadTexture("./Resources/cursor.png");

	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();
	cursor_->textureID = DrawerManager::GetInstance()->GetTexture("./Resources/cursor.png");
	cursor_->transform.scale = { 32.0f,32.0f };
	offset_ = { 13.0f,-17.0f };
	time_ = 0;
}

void Cursor::Update() {
#ifdef _DEBUG
	ImGui::Begin("Cursor");
	ImGui::DragFloat2("Offset", &offset_.x);
	ImGui::End();
#endif // _DEBUG
	auto mouse = Input::GetInstance()->GetMouse();
	static uint32_t kMaxTime = 3;
	if (mouse->Pushed(Mouse::Button::Left) || mouse->LongPush(Mouse::Button::Left)) {
		cursor_->transform.rotate = Quaternion::MakeRotateZAxis(-15.0f * Lamb::Math::toDegree<float>);
		time_ = kMaxTime;
	}
	if (time_ <= 0) {
		cursor_->transform.rotate = Quaternion::MakeRotateZAxis(0.0f);
	}
	else {
		time_--;
	}
	cursor_->transform.translate = ChangeCursorToTexturePos();
	cursor_->transform.translate += offset_;
	cursor_->transform.CalcMatrix();
}

void Cursor::Draw(const Camera& camera) {
	tex2D_->Draw(
		cursor_->transform.GetMatrix(),
		Mat4x4::kIdentity,
		camera.GetViewOthographics(),
		cursor_->textureID,
		cursor_->color,
		BlendType::kUnenableDepthNone
	);
}

void Cursor::Draw(const Mat4x4& cameraMat) {
	tex2D_->Draw(
		cursor_->transform.GetMatrix(),
		Mat4x4::kIdentity,
		cameraMat,
		cursor_->textureID,
		cursor_->color,
		BlendType::kUnenableDepthNone
	);
}

Vector2 Cursor::ChangeCursorToTexturePos() {
	auto mouse = Input::GetInstance()->GetMouse();
	return Vector2((mouse->GetPos().x - 1280.0f * 0.5f), mouse->GetPos().y * -1.0f + 720.0f * 0.5f);
}
