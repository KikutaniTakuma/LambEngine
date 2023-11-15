#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>
#include <format>

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	for (auto& i : resultString_) {
		i.SetFormat("./Resources/Font/default.spritefont");
	}
	Vector3 from0 = Vector3{ 1.0f, 0.7f, 0.5f }.Normalize();
	Vector3 to0 = -from0;
	Vector3 from1 = Vector3{ -0.6f, 0.9f, 0.2f }.Normalize();
	Vector3 to1 = Vector3{ 0.4f, 0.7f, -0.5f }.Normalize();

	std::array<Mat4x4, 3> rotateMatrix = { 
		DirectionToDirection(Vector3::xIdy.Normalize(), Vector3{-1.0f,0.0f,0.0f}.Normalize()),
		DirectionToDirection(from0, to0),
		DirectionToDirection(from1, to1)
	};

	for (size_t i = 0; i < rotateMatrix.size(); i++) {
		resultString_[i] << "rotateMatrix" << i << "\n" << GetMatrixString(rotateMatrix[i]);
		resultString_[i].scale_ *= 0.3f;
		resultString_[i].pos_.y = 110.0f * static_cast<float>(i);
	}
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	for (size_t i = 0; i < resultString_.size(); i++) {
		resultString_[i].Debug("resultString" + std::to_string(i));
	}
}

void GameScene::Draw() {
	for (auto& i : resultString_) {
		i.Draw();
	}
}