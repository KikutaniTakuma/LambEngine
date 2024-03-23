#include "GameScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
#include "Engine/Graphics/PipelineObject/WaterPipeline/WaterPipeline.h"
#include "Game/Water/Water.h"
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut/ScreenOut.h"

#include "Math/Matrix.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 100000.0f;
	//camera_->pos.y = 15.0f;
	camera_->pos.z = -5.0f;
	camera_->rotate.x = 0.030f;
	camera_->farClip = 1000.0f * 1000.0f;
	camera_->pos.y = 6360018.000f;

	skydome_.reset(new SkyDome());

	skydome_->Initialize();

	sun_.reset(new Texture2D("./Resources/Ball.png"));

	sun_->scale *= 10000.0f;
	sun_->color = 0xdf0000ff;

	earth_.reset(new Model("./Resources/Cube.obj"));

	earth_->scale *= 6360.0f * 1000.0f;
	earth_->color = 0x84331Fff;
}

void GameScene::Finalize() {
	skydome_->Finalize();
}

void GameScene::Update() {
	camera_->Debug("カメラ");
	camera_->Update();

	skydome_->Debug("skyDome");
	skydome_->Upadate();

	sun_->pos = skydome_->GetSunPos();
	sun_->rotate = camera_->rotate;
	sun_->Debug("sun");
	sun_->Update();

	earth_->Update();
}

void GameScene::Draw() {
	skydome_->Draw(*camera_);

	sun_->Draw(camera_->GetViewProjection(), Pipeline::Normal, false);

	earth_->Draw(camera_->GetViewProjection(), camera_->GetPos());

	Lamb::screenout << skydome_->GetSunPos();
}