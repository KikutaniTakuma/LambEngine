#pragma once

#include <array>
#include <memory>
#include <string>

#include "GlobalVariables/GlobalVariables.h"
#include "Utils/Camera/Camera.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"

class Camera;
class FollowCamera {
private:
	struct Target {
		Vector3 position;
		Vector3 rotate;
	};

	enum FloatParameter {

		kFloatParameterCount,
	};
	enum Vector2Parameter {
		kCamaraSpeed,
		kCameraRotateDeray,


		kVector2ParameterCount,
	};
	enum Vector3Parameter {
		kOffset,
		kCameraMoveDeray,

		kVector3ParameterCount,
	};
public:
	FollowCamera();
	void Initialize();
	void Update();

	void SetTarget(const Vector3& position, const Vector3& rotate) {
		target_->position = position;
		target_->rotate = rotate;
	}
	void SetCamera(Camera* camera) { camera_ = camera; }

	void SetDestinationAngle(const Vector3& angle) { destinationAngle_ = angle; }

	bool IsUsedCamera() { return isUsedCamera_; }

	Vector2 GetPrePos() { return prePos_; }
private:
	void UpdateRotate();
	void GamePad();
	void Keyboard();
	void Mouse();

	float LerpShortAngle(float a, float b, float t);

	void SetGlobalVariable();
	void ApplyGlobalVariable();

	Target* target_;
	Camera* camera_;
	// カメラスピード
	Vector2 cameraSpeed_;

	// 現在のカメラ
	Vector3 interTarget_;
	// 回転の終点
	Vector3 destinationAngle_;

	bool isUsedCamera_;

	Vector2 prePos_;
#pragma region GlobalVariables
	std::unique_ptr<GlobalVariables> globalVariables_;
	std::string name_;
	std::array<float, FloatParameter::kFloatParameterCount> floatParameter_;
	std::array<Vector2, Vector2Parameter::kVector2ParameterCount> vector2Parameter_;
	std::array<Vector3, Vector3Parameter::kVector3ParameterCount> vector3Parameter_;

	std::array<std::string, FloatParameter::kFloatParameterCount> floatParameterItemNames_ = {
	};
	std::array<std::string, Vector2Parameter::kVector2ParameterCount> vector2ParameterItemNames_ = {
		"CameraSpeed",
		"CameraRotateDelay",
	};
	std::array<std::string, Vector3Parameter::kVector3ParameterCount> vector3ParameterItemNames_ = {
		"Offset",
		"CameraMoveDelay",
	};
#pragma endregion
};