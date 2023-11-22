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
	camera_.farClip = 3000.0f;
	camera_.pos.z = -5.0f;
	camera_.pos.y = 1.1f;

	watame_.ThreadLoadObj("./Resources/Hololive/Watame/Watame.obj");
	towa_.ThreadLoadObj("./Resources/Hololive/Towa/Obj/Towa.obj");
	kanata_.ThreadLoadObj("./Resources/Hololive/Kanata/Obj/Kanata.obj");
	luna_.ThreadLoadObj("./Resources/Hololive/Luna/Obj/Luna.obj");

	//pera_.Initialize("./Resources/Shaders/PostShader/Post.VS.hlsl", "./Resources/Shaders/PostShader/PostNone.PS.hlsl");
	pera_.scale_ = { 1280.0f,720.0f };
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_.Debug("camera");
	watame_.Debug("watame");
	watame_.Update();
	towa_.Debug("towa");
	towa_.Update();
	kanata_.Debug("kanata");
	kanata_.Update();
	luna_.Debug("luna");
	luna_.Update();
	pera_.Update();
}

void GameScene::Draw() {
	camera_.Update(Vector3::zero);
	//pera_.PreDraw();

	Lamb::screenout << "Test Hololive Models" << Lamb::endline;
	Lamb::screenout << "4th gen";

	watame_.Draw(camera_.GetViewProjection(), camera_.GetPos());
	towa_.Draw(camera_.GetViewProjection(), camera_.GetPos());
	kanata_.Draw(camera_.GetViewProjection(), camera_.GetPos());
	luna_.Draw(camera_.GetViewProjection(), camera_.GetPos());

	//pera_.Draw(camera_.GetViewOthographics(), Pipeline::None);
}