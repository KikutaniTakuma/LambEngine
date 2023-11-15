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
	Quaternion q1 = { 2.0f,3.0f,4.0f,1.0f };
	Quaternion q2 = { 1.0f,3.0f,5.0f,2.0f };
	Quaternion identity = Quaternion::identity;
	Quaternion conj = q1.Conjugate();
	Quaternion inv = q1.Inverce();
	Quaternion normal = q1.Normalize();
	Quaternion mul1 = q1 * q2;
	Quaternion mul2 = q2 * q1;
	float norm = q1.Length();

	resultString_ 
		<< identity << " : identity" << "\n"
		<< conj << " : Conjugate" << "\n"
		<< inv << " : Inverce" << "\n"
		<< normal << " : Normalize" << "\n"
		<< mul1 << " : q1 * q2" << "\n"
		<< mul2 << " : q2 * q1" << "\n"
		<< norm << " : Norm";
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	resultString_.Debug("result");
}

void GameScene::Draw() {
	resultString_.Draw();
}