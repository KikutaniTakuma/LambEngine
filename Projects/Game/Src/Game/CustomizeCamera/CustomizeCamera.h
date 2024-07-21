#pragma once

#include <array>
#include <memory>
#include <string>

#include "GlobalVariables/GlobalVariables.h"
#include "Utils/Camera/Camera.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"

class Camera;
class CustomizeCamera {
private:
	struct Target {
		Vector3 position;
	};

	enum FloatParameter {
		kZoomSpeed,

		kFloatParameterCount,
	};
	enum Vector2Parameter {
		kRotateSpeed,

		kVector2ParameterCount,
	};
	enum Vector3Parameter {
		kOffset,
		kCameraMoveDelay,
		kVector3ParameterCount,
	};
public:
	CustomizeCamera();
	void Initialize();
	void Update();

	void Reset();

	void SetTarget(const Vector3& position) {
		target_->position = position;
	}
	void SetCamera(Camera* camera) { camera_ = camera; }
private:
	void UpdateRotate();
	void GamePad();
	void Keyboard();

	float LerpShortAngle(float a, float b, float t);

	void SetGlobalVariable();
	void ApplyGlobalVariable();

	Target* target_ = nullptr;
	Camera* camera_ = nullptr;
	// カメラスピード
	Vector2 cameraSpeed_;

	// 現在のカメラ
	Vector3 interTarget_;
	// 回転の終点
	Vector3 destinationAngle_;
	// オフセット
	Vector3 offset_;
#pragma region GlobalVariables
	std::unique_ptr<GlobalVariables> globalVariables_;
	std::string name_;
	std::array<float, FloatParameter::kFloatParameterCount> floatParameter_;
	std::array<Vector2, Vector2Parameter::kVector2ParameterCount> vector2Parameter_;
	std::array<Vector3, Vector3Parameter::kVector3ParameterCount> vector3Parameter_;

	std::array<std::string, FloatParameter::kFloatParameterCount> floatParameterItemNames_ = {
		"ZoomSpeed",
	};
	std::array<std::string, Vector2Parameter::kVector2ParameterCount> vector2ParameterItemNames_ = {
		"RotateSpeed",

	};
	std::array<std::string, Vector3Parameter::kVector3ParameterCount> vector3ParameterItemNames_ = {
		"Offset",
		"CameraMoveDelay",
	};

#pragma endregion
};