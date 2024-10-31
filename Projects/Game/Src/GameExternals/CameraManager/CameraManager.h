#pragma once

#include <memory>

#include "GameExternals/CustomizeCamera/CustomizeCamera.h"
#include "GameExternals/ChargeCamera/ChargeCamera.h"
#include "GameExternals/DebugCamera/DebugCamera.h"
#include "GameExternals/GoalCamera/GoalCamera.h"
#include "GameExternals/FixedCamera/FixedCamera.h"
#include "GameExternals/FollowCamera/FollowCamera.h"

class Camera;
class Player;
class CameraManager {
public:
	enum Type {
		kCustomize,
		kCharge,
		kFollow,
		kGoal,
		kDebug,
		kFixed,
		kCount,
	};

	static CameraManager* GetInstance();
	
	void Initialize(Camera* camera, Player* player, const Vector3& pos, const Vector3& rotate, const Vector3& goalScale, const Vector3 goalRotate, const Vector3 goalPos);

	void Update(const Vector3& pos, const Vector3& rotate);

	Type GetType() { return type_; }
	Type GetPreType() { return preType_; }
	void SetType(Type type) { 
		preType_ = type_;
		type_ = type; }
	CustomizeCamera* GetCustomizeCamera() { return customizeCamera_.get(); }
	GoalCamera* GetGoalCamera() {return goalCamera_.get();}
	ChargeCamera* GetChargeCamera() {return chargeCamera_.get();}
	FollowCamera* GetFollowCamera() { return followCamera_.get(); }
private:
	Camera* camera_;
	std::unique_ptr<CustomizeCamera> customizeCamera_;
	std::unique_ptr<ChargeCamera> chargeCamera_;
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<GoalCamera> goalCamera_;
	std::unique_ptr<FollowCamera> followCamera_;
	std::unique_ptr<FixedCamera> fixedCamera_;

	Type type_;
	Type preType_;
};