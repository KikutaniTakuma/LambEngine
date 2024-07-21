#pragma once

#include "Math/Vector3.h"
#include "Utils/Easing/Easing.h"

class Camera;
class GoalCamera {
private:
	struct Goal {
		Vector3 scale;
		Vector3 rotate;
		Vector3 pos;
	};

	struct CameraData {
		Vector3 rotate;
		Vector3 pos;
	};
public:
	void Initialize();
	void Update();

	void SetGoal(const Vector3& scale,const Vector3& rotate, const Vector3& pos) {
		goal_.scale = scale;
		goal_.pos = pos;
		goal_.rotate = rotate;
	}
	void SetStartCamera(const Vector3& pos, const Vector3& rotate) {
		startCamera_.pos = pos;
		startCamera_.rotate = rotate;
	}
	void SetCamera(Camera* camera) { camera_ = camera; }
private:
	Vector3 LerpShortAngle(const Vector3& a, const Vector3& b);
	Camera* camera_;
	Goal goal_;
	CameraData startCamera_;
	CameraData endCamera_;
	Vector3 offset_;
	Vector3 rotateOffset_;
	float easingCount_;
	Easing rotateEasing_;
	Easing translateEasing_;
};