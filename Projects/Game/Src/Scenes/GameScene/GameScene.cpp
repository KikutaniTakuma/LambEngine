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
	
	Vector3 from0 = Vector3{ 1.0f, 0.7f, 0.5f }.Normalize();
	Vector3 to0 = -from0;
	Vector3 from1 = Vector3{ -0.6f, 0.9f, 0.2f }.Normalize();
	Vector3 to1 = Vector3{ 0.4f, 0.7f, -0.5f }.Normalize();

	std::array<Mat4x4, 3> rotateMatrix = {
		DirectionToDirection(Vector3::kXIndentity,Vector3{-1.0f,0.0f,0.0f}),
		DirectionToDirection(from0, to0),
		DirectionToDirection(from1, to1)
	};

	std::array<Quaternion, 3> rotateQuaternion = {
		Quaternion::DirectionToDirection(Vector3::kXIndentity,Vector3{-1.0f,0.0f,0.0f}),
		Quaternion::DirectionToDirection(from0, to0),
		Quaternion::DirectionToDirection(from1, to1)
	};

	for (size_t i = 0; i < rotateMatrix.size();i++) {
		Lamb::screenout << "rotateMatrix" << i << Lamb::endline
			<< GetMatrixString(rotateMatrix[i]);
	}

	for (size_t i = 0; i < rotateQuaternion.size(); i++) {
		Lamb::screenout << "rotateQuaternion" << i << Lamb::endline
			<< GetMatrixString(rotateQuaternion[i].GetMatrix());
	}

	model_->Draw(camera_->GetViewProjection(), camera_->GetPos());
}