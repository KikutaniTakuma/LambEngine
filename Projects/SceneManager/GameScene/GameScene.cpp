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

	Quaternion rotation = Quaternion::MakeRotateAxisAngle(Vector3{ 0.0f,0.0f,1.0f }.Normalize(), 0.45f);
	Quaternion rotation2 = Quaternion::MakeRotateZAxis(0.45f);
	Vector3 pointY = { 2.1f, -0.9f, 1.3f };
	Mat4x4 rotateMatrix = rotation.GetMatrix();
	Mat4x4 rotateMatrix2 = rotation2.GetMatrix();
	Vector3 rotateByQuaternion = pointY * rotation;
	Vector3 rotateByMatrix = pointY * rotateMatrix;
	Vector3 rotateByQuaternion2 = pointY * rotation2;
	Vector3 rotateByMatrix2 = pointY * rotateMatrix2;

	resultString_ << rotation << " : rotation\n"
		<< "rotateMatrix\n"
		<< GetMatrixString(rotateMatrix)
		<< rotateByQuaternion << " : rotateByQuaternion\n"
		<< rotateByMatrix << " : rotateByMatrix\n"
		<< "rotateMatrix2\n"
		<< GetMatrixString(rotateMatrix2)
		<< rotateByQuaternion2 << " : rotateByQuaternion\n"
		<< rotateByMatrix2 << " : rotateByMatrix";
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	resultString_.Debug("result");
}

void GameScene::Draw() {
	resultString_.Draw();
}