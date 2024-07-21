#include "Cursor.h"

#include <numbers>

#include "imgui.h"
#include "Input/Input.h"

void Cursor::Initialize() {
	cursor_ = std::make_unique<Texture2D>();
	cursor_->LoadTexture("./Resources/cursor.png");
	cursor_->scale = { 32.0f,32.0f };
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
		cursor_->rotate.z = -15.0f * std::numbers::pi_v<float> / 180.0f;
		time_ = kMaxTime;
	}
	if (time_ <= 0) {
		cursor_->rotate.z = 0;
	}
	else {
		time_--;
	}
	cursor_->pos = ChangeCursorToTexturePos();
	cursor_->pos += offset_;
	cursor_->Update();
}

void Cursor::Draw(const Camera& camera) {
	cursor_->Draw(camera.GetOthographics(), Pipeline::Normal, false, false);
}

void Cursor::Draw(const Mat4x4& cameracameraMat) {
	cursor_->Draw(cameracameraMat, Pipeline::Normal, false, false);
}

Vector2 Cursor::ChangeCursorToTexturePos() {
	auto mouse = Input::GetInstance()->GetMouse();
	return Vector2((mouse->GetPos().x - 1280.0f * 0.5f), mouse->GetPos().y * -1.0f + 720.0f * 0.5f);
}
