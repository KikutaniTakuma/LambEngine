#include "Camera.h"
#include "Engine/Engine.h"
#include "Input/KeyInput/KeyInput.h"
#include "Input/Mouse/Mouse.h"
#include "Input/Gamepad/Gamepad.h"
#include "imgui.h"
#include "Engine/Core/WinApp/WinApp.h"
#include <algorithm>
#include <numbers>
#include <cmath>

Camera::Camera() noexcept :
	pos(),
	scale(Vector3::identity),
	rotate(),
	drawScale(1.0f),
	farClip(1000.0f),
	fov(0.45f),
	view(),
	projection(),
	othograohics()
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
	view.Affin(scale, rotate, pos);
	worldPos = { view[0][3],view[1][3], view[2][3] };
	view.Inverse();

	static auto engine = Engine::GetInstance();
	static const float aspect = static_cast<float>(engine->clientWidth) / static_cast<float>(engine->clientHeight);

	const auto&& windowSize = WinApp::GetInstance()->GetWindowSize();


	fov = std::clamp(fov, 0.0f, 1.0f);
	projection.PerspectiveFov(fov, aspect, kNearClip, farClip);
	viewProjecction = view * projection;

	viewProjecctionVp = viewProjecction * MakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f);

	othograohics.Orthographic(
		-static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
		static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
		static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
		-static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
		kNearClip, farClip);
	viewOthograohics = view * othograohics;


	viewOthograohicsVp = viewOthograohics * MakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f);
}

void Camera::Update(const Vector3& gazePoint) {
	Vector3 offset = pos - gazePoint;

	offset *= MakeMatrixRotate(rotate);

	view.Affin(scale, rotate, offset);
	worldPos = { view[0][3],view[1][3], view[2][3] };
	view.Inverse();

	static auto engine = Engine::GetInstance();
	static const float aspect = static_cast<float>(engine->clientWidth) / static_cast<float>(engine->clientHeight);

	const auto&& windowSize = WinApp::GetInstance()->GetWindowSize();


	fov = std::clamp(fov, 0.0f, 1.0f);
	projection.PerspectiveFov(fov, aspect, kNearClip, farClip);
	viewProjecction = view * projection;

	viewProjecctionVp = viewProjecction * MakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f);

	othograohics.Orthographic(
		-static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
		static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
		static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
		-static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
		kNearClip, farClip);
	viewOthograohics = view * othograohics;


	viewOthograohicsVp = viewOthograohics * MakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f);
}

void Camera::Update(const Mat4x4& worldMat) {
	view.Affin(scale, rotate, pos);
	view = worldMat * view;
	worldPos = { view[0][3],view[1][3], view[2][3] };
	view.Inverse();

	static auto engine = Engine::GetInstance();
	static const float aspect = static_cast<float>(engine->clientWidth) / static_cast<float>(engine->clientHeight);

	const auto&& windowSize = WinApp::GetInstance()->GetWindowSize();

	fov = std::clamp(fov, 0.0f, 1.0f);
	projection.PerspectiveFov(fov, aspect, kNearClip, farClip);
	viewProjecction = view * projection;

	viewProjecctionVp = viewProjecction * MakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f);

	othograohics.Orthographic(
		-static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
		static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
		static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
		-static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
		kNearClip, farClip);
	viewOthograohics = view * othograohics;


	viewOthograohicsVp = viewOthograohics * MakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f);
}

void Camera::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("pos", &pos.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
	ImGui::End();
#endif // _DEBUG
}