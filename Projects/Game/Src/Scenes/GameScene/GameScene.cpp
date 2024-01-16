#include "GameScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
#include "Utils/ScreenOut/ScreenOut.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	camera_->rotate.x = 0.04f;
	camera_->offset.z = -60.0f;
	camera_->offset.y = 8.0f;

	water_ = Water::GetInstance();
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");

	//water_->Debug("water");
	water_->Update();
}

void GameScene::Draw() {
	Quaternion rotation0 = Quaternion::MakeRotateAxisAngle(Vector3{ 0.71f,0.71f,0.0f }.Normalize(), 0.3f);
	Quaternion rotation1 = -rotation0;

	std::array<float, 5> lerpT = {
		0.0f,0.3f,0.5f,0.7f,1.0f
	};

	std::array<Quaternion, 5> interpolate = {
		Quaternion::Slerp(rotation0, rotation1, lerpT[0]),
		Quaternion::Slerp(rotation0, rotation1, lerpT[1]),
		Quaternion::Slerp(rotation0, rotation1, lerpT[2]),
		Quaternion::Slerp(rotation0, rotation1, lerpT[3]),
		Quaternion::Slerp(rotation0, rotation1, lerpT[4])
	};

	for (size_t i = 0llu; i < interpolate.size(); i++) {
		Lamb::screenout << interpolate[i] << "   : interpolate" << i << ", Slerp(q0, q1, " << lerpT[i] << "f)\n";
	}
}