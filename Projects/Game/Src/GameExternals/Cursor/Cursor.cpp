#include "Cursor.h"

#include <numbers>

#include "imgui.h"
#include "Input/Input.h"

void Cursor::Initialize() {
	cursor_ = std::make_unique<Tex2DInstance>();
	cursor_->Load("./Resources/cursor.png");
	cursor_->scale = { 32.0f,32.0f };
	offset_ = { 13.0f,-17.0f };
	time_ = 0;
}

void Cursor::Update() {
#ifdef USE_DEBUG_CODE
	ImGui::Begin("Cursor");
	ImGui::DragFloat2("Offset", &offset_.x);
	ImGui::End();
#endif // USE_DEBUG_CODE
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
}

void Cursor::Draw(const Camera& camera) {
	cursor_->blend = BlendType::kUnenableDepthNormal;
	cursor_->Draw(camera.GetOthographics());
}

void Cursor::Draw(const Mat4x4& cameracameraMat) {
	cursor_->blend = BlendType::kUnenableDepthNormal;
	cursor_->Draw(cameracameraMat);
}

Vector2 Cursor::ChangeCursorToTexturePos() {
	auto mouse = Input::GetInstance()->GetMouse();
	Vector3 texPos = Vector3(mouse->GetPos(), 0.0f) * (Mat4x4::MakeTranslate(Vector3::kZIdentity * 10.0f) * Mat4x4::MakeOrthographic(1280.0f, 720.0f, 0.1f, 1000.0f) * Mat4x4::MakeViewPort(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f,1.0f)).Inverse();
	return {texPos.x, texPos.y};
}
