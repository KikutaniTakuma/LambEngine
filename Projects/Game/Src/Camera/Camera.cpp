#include "Camera.h"
#include "Utils/SafePtr.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Input/Input.h"
#include "imgui.h"
#include <algorithm>
#include <numbers>
#include <cmath>

Mat4x4 Camera::kViewProjection;
Mat4x4 Camera::kViewOthographics;

void Camera::InitStaticMatrix() {
	Lamb::SafePtr window = WindowFactory::GetInstance();
	Vector2 clientSize = window->GetClientSize();
	const float aspect = clientSize.x / clientSize.y;

	kViewOthographics = Mat4x4::MakeOrthographic(
		clientSize.x, clientSize.y,
		kNearClip_, 1000.0f
	);
	kViewProjection = Mat4x4::MakePerspectiveFov(
		0.45f, aspect,
		kNearClip_, 1000.0f
	);
}

Camera::Camera() noexcept :
	pos(),
	scale(Vector3::kIdentity),
	rotate(),
	drawScale(1.0f),
	farClip(1000.0f),
	fov(0.45f),
	view_(),
	projection_(),
	othograohics_(),
	offset{ Vector3::kZIdentity * -10.0f }
{}

Camera::Camera(const Camera& right) noexcept
{
	*this = right;
}

Camera::Camera(Camera&& right) noexcept
{
	*this = std::move(right);
}

void Camera::Update() {
	view_ = Mat4x4::MakeAffin(scale, rotate, pos);
	worldPos_ = pos;
	view_ = view_.Inverse();

	Vector2 clientSize = WindowFactory::GetInstance()->GetClientSize();
	const float aspect = clientSize.x / clientSize.y;


	fov = std::clamp(fov, 0.0f, 1.0f);
	projection_ = Mat4x4::MakePerspectiveFov(fov, aspect, kNearClip_, farClip);
	viewProjecction_ = view_ * projection_;

	viewProjecctionVp_ = viewProjecction_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);

	othograohics_ = Mat4x4::MakeOrthographic(
		clientSize.x * drawScale,
		clientSize.y * drawScale,
		kNearClip_, farClip);
	viewOthograohics_ = view_ * othograohics_;


	viewOthograohicsVp_ = viewOthograohics_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);
}

void Camera::Update(const Vector3& gazePoint) {
	pos = gazePoint + (offset * Mat4x4::MakeRotate(rotate));


	Update();
}

void Camera::Update(const Mat4x4& worldMat) {
	view_ = Mat4x4::MakeAffin(scale, rotate, pos);
	view_ = worldMat * view_;
	worldPos_ = { view_[0][3],view_[1][3], view_[2][3] };
	view_ = view_.Inverse();

	Vector2 clientSize = WindowFactory::GetInstance()->GetClientSize();
	const float aspect = clientSize.x / clientSize.y;

	fov = std::clamp(fov, 0.0f, 1.0f);
	projection_ = Mat4x4::MakePerspectiveFov(fov, aspect, kNearClip_, farClip);
	viewProjecction_ = view_ * projection_;

	viewProjecctionVp_ = viewProjecction_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);

	othograohics_ = Mat4x4::MakeOrthographic(
		clientSize.x * drawScale,
		clientSize.y * drawScale,
		kNearClip_, farClip);
	viewOthograohics_ = view_ * othograohics_;


	viewOthograohicsVp_ = viewOthograohics_ * Mat4x4::MakeViewPort(0.0f, 0.0f, clientSize.x, clientSize.y, 0.0f, 1.0f);
}

void Camera::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef USE_DEBUG_CODE
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("pos", pos.data(), 0.01f);
	ImGui::DragFloat3("scale", scale.data(), 0.01f);
	ImGui::DragFloat3("rotate", rotate.data(), 0.01f);
	ImGui::DragFloat3("offset", offset.data(), 0.01f);
	ImGui::End();
#endif // USE_DEBUG_CODE
}