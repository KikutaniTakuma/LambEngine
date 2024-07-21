#include "CameraManager.h"

#include "Game/Player/Player.h"
#include "Utils/Camera/Camera.h"

CameraManager* CameraManager::GetInstance() {
	static CameraManager instance;
	return &instance;
}

void CameraManager::Initialize(Camera* camera, Player* player, const Vector3& pos, const Vector3& rotate, const Vector3& goalScale, const Vector3 goalRotate, const Vector3 goalPos) {
	camera_ = camera;

	customizeCamera_ = std::make_unique<CustomizeCamera>();
	chargeCamera_ = std::make_unique<ChargeCamera>();
	debugCamera_ = std::make_unique<DebugCamera>();
	goalCamera_ = std::make_unique<GoalCamera>();
	followCamera_ = std::make_unique<FollowCamera>();
	fixedCamera_ = std::make_unique<FixedCamera>();

	customizeCamera_->SetCamera(camera_);
	customizeCamera_->SetTarget(pos);
	customizeCamera_->Initialize();

	chargeCamera_->SetCamera(camera_);
	chargeCamera_->SetFollowCamera(followCamera_.get());
	chargeCamera_->SetPlayer(player);
	chargeCamera_->Initialize();

	goalCamera_->SetCamera(camera_);
	goalCamera_->SetGoal(goalScale, goalRotate, goalPos);
	goalCamera_->Initialize();

	followCamera_->SetTarget(pos, rotate);
	followCamera_->SetCamera(camera_);
	followCamera_->Initialize();

	fixedCamera_->SetStartGoal({ 0.0f,0.0f,0.0f }, goalPos);
	fixedCamera_->SetCamera(camera_);
	fixedCamera_->Initialize();

}

void CameraManager::Update(const Vector3& pos, const Vector3& rotate) {
	switch (type_) {
	case CameraManager::kCustomize:
		customizeCamera_->Update();
		break;
	case CameraManager::kCharge:
		chargeCamera_->Update();
		break;
	case CameraManager::kFollow:
		followCamera_->SetTarget(pos, rotate);
		followCamera_->Update();
		break;
	case CameraManager::kGoal:
		goalCamera_->Update();
		break;
	case CameraManager::kDebug:
		debugCamera_->Update(camera_);
		break;
	case CameraManager::kFixed:
		fixedCamera_->Update();
		break;
	case CameraManager::kCount:
		break;
	default:
		break;
	}

}
