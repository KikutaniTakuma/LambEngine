#include "DebugCamera.h"

#include <numbers>

#include "Input/Input.h"
#include "Math/Matrix.h"
#include "Math/Vector3.h"
#include "Camera/Camera.h"


void DebugCamera::Update(Camera* camera) {
	auto input = Input::GetInstance()->GetMouse();
	float wheel = input->GetWheel();
	Vector2 mouseMove = input->GetVelocity();
	if (input->LongPush(Mouse::Button::Right)) {
		float rot = std::numbers::pi_v<float> / 180.0f;
		camera->rotate.x -= rot * mouseMove.y * 0.1f;
		camera->rotate.y += rot * mouseMove.x * 0.1f;
	}
	else if (input->LongPush(Mouse::Button::Middle)) {
		Mat4x4 rotMat = Mat4x4::MakeRotate(camera->rotate);
		Vector3 cameraX = Vector3(rotMat[0][0], rotMat[0][1], rotMat[0][2]) * static_cast<float>(-mouseMove.x) * 0.01f;
		Vector3 cameraY = Vector3(rotMat[1][0], rotMat[1][1], rotMat[1][2]) * static_cast<float>(-mouseMove.y) * 0.01f;
		camera->pos += cameraX + cameraY;
	}
	else if (wheel != 0) {
		Mat4x4 rotMat = Mat4x4::MakeRotate(camera->rotate);
		Vector3 cameraZ = Vector3(rotMat[2][0], rotMat[2][1], rotMat[2][2]) * (static_cast<float>(wheel / 120.0f));
		camera->pos += cameraZ;
	}
	camera->Update();
}
