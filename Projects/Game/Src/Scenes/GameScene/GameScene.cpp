#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Utils/Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	camera_->pos.y = 1.1f;

	model_ = std::make_unique<Model>();

	model_->ThreadLoadObj("./Resources/Hololive/Watame/Watame.obj");
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");
	model_->Update();
}

void GameScene::Draw() {
	camera_->Update(Vector3::kZero);
	
	Quaternion rotation = Quaternion::MakeRotateAxisAngle(Vector3{ 0.0f,0.0f,1.0f }.Normalize(), 0.45f);
	Quaternion rotation2 = Quaternion::MakeRotateZAxis(0.45f);
	Vector3 pointY = { 2.1f, -0.9f, 1.3f };
	Mat4x4 rotateMatrix = rotation.GetMatrix();
	Mat4x4 rotateMatrix2 = rotation2.GetMatrix();
	Vector3 rotateByQuaternion = pointY * rotation;
	Vector3 rotateByMatrix = pointY * rotateMatrix;
	Vector3 rotateByQuaternion2 = pointY * rotation2;
	Vector3 rotateByMatrix2 = pointY * rotateMatrix2;

	Lamb::screenout <<
		rotation << " : rotation" << Lamb::endline
		<< "rotateMatrix" << Lamb::endline
		<< GetMatrixString(rotateMatrix)
		<< rotateByQuaternion << " : rotateByQuaternion" << Lamb::endline
		<< rotateByMatrix << " : rotateByMatrix" << Lamb::endline
		<< "rotateMatrix2" << Lamb::endline
		<< GetMatrixString(rotateMatrix2)
		<< rotateByQuaternion2 << " : rotateByQuaternion" << Lamb::endline
		<< rotateByMatrix2 << " : rotateByMatrix";

	model_->Draw(camera_->GetViewProjection(), camera_->GetPos());
}