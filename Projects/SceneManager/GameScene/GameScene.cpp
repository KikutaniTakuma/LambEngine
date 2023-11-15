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
	Quaternion rotation = Quaternion::MakeRotateAxisAngle(Vector3{1.0f,0.4f,-0.2f}.Normalize(), 0.45f);
	Vector3 pointY = { 2.1f, -0.9f, 1.3f };
	Mat4x4 rotateMatrix = rotation.GetMatrix();
	Vector3 rotateByQuaternion = pointY * rotation;
	Vector3 rotateByMatrix = pointY * rotateMatrix;

	resultString_ << rotation << " : rotation\n"
		<< "rotateMatrix\n"
		<< GetMatrixString(rotateMatrix)
		<< rotateByQuaternion << " : rotateByQuaternion\n"
		<< rotateByMatrix << " : rotateByMatrix";
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	resultString_.Debug("result");
}

void GameScene::Draw() {
	resultString_.Draw();
}