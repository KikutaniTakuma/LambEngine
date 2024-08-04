#pragma once

#include "Math/Vector3.h"

class Camera;
class FixedCamera {
public:
	FixedCamera();
	void SetStartGoal(const Vector3& start, const Vector3& goal) { 
		startPos_ = start;
		goalPos_ = goal;
	}
	void SetCamera(Camera* camera) { camera_ = camera; }
	void Initialize();
	void Update();
private:
	Camera* camera_;
	Vector3 startPos_;
	Vector3 goalPos_;
};