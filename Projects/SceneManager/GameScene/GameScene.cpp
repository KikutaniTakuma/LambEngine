#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include "Utils/Math/Quaternion.h"
#include <numbers>
#include <format>

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	resultString_.SetFormat("./Resources/Font/default.spritefont");
	resultString_.scale_ *= 0.3f;
	Quaternion rotation0 = Quaternion::MakeRotateAxisAngle(Vector3{0.71f,0.71f,0.0f}.Normalize(), 0.3f);
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
		resultString_ << interpolate[i] << "   : interpolate" << i << ", Slerp(q0, q1, " << lerpT[i] << "f)\n";
	}
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	resultString_.Debug("result");
}

void GameScene::Draw() {
	resultString_.Draw();
}