#pragma once
#include <vector>

#include "Math/Vector3.h"
#include "Utils/Easing/Easing.h"


class Camera;
class Player;
class FollowCamera;
class ChargeCamera {
private:
	struct EasingData {
		Vector3 start;
		Vector3 end;
		float time;
		bool isUsed;
		Easing easing;
	};
public:
	ChargeCamera();

	void Initialize();

	void Update();

	void SetStartCamera(const Vector3& pos, const Vector3& rotate);
	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetPlayer(Player* player) { player_ = player; }
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }

	bool GetIsSceneChange() {return isSceneChange_;}
private:
	Vector3 LerpShortAngle(const Vector3& a, const Vector3& b);
	void Reset();
	Camera* camera_;
	FollowCamera* followCamera_;
	Player* player_;
	std::vector<EasingData> easingTranslateData_;
	std::vector<EasingData> easingRotateData_;

	EasingData customizeToChargeTranslate_;
	EasingData customizeToChargeRotate_;

	bool isSceneChange_;
};