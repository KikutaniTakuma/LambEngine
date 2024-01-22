#include "TitleScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
#include "Utils/ScreenOut/ScreenOut.h"

TitleScene::TitleScene() :
	BaseScene(BaseScene::ID::Game)
{}

void TitleScene::Initialize() {
	camera_->pos.z = -5.0f;
	camera_->pos.y = 1.0f;

	model_.reset(new Model{});
	model_->LoadObj("./Resources/Hololive/Watame/Watame.obj");
}

void TitleScene::Finalize() {

}

void TitleScene::Update() {
	
	model_->Debug("model");
	model_->Update();
}

void TitleScene::Draw() {
	camera_->Update(Vector3::kZero);
	model_->Draw(camera_->GetViewProjection(), camera_->GetPos());

	Lamb::screenout << "Title Scnene" << Lamb::endline;
}