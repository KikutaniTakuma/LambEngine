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
	Vector3 axis = Vector3::identity.Normalize();
	float angle = 0.44f;
	Mat4x4 result = MakeRotateAxisAngle(axis, angle);
	resultString_.SetFormat("./Resources/Font/default.spritefont");
	resultString_ << "rotateMatrix\n" << GetMatrixString(result);
	resultString_.scale_ *= 0.5f;
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	resultString_.Debug("result");
}

void GameScene::Draw() {
	resultString_.Draw();
}