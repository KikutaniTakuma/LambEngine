#include "TestScene.h"
#include "Level/LevelLoader.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"
#include "Game/Water/Water.h"

TestScene::TestScene():
	BaseScene(BaseScene::ID::Test),
	fishes_()
{
	water_ = Water::GetInstance();
}

void TestScene::Load()
{
	objectManager_ = ObjectManager::GetInstance();
	objectManager_->Load("./SceneData/waterTest.json");
}

void TestScene::Initialize() {
	fishes_ = std::make_unique<Fishes>();
	fishes_->Init(200);
}

void TestScene::Finalize() {
	fishes_.reset();
}

void TestScene::Update()
{
	objectManager_->Update();

	fishes_->Update();

	water_->Debug("water");

	water_->Update(objectManager_->GetCameraPos());
}

void TestScene::Draw()
{
	water_->Draw(objectManager_->GetCameraMatrix());
	objectManager_->Draw();

	fishes_->Draw(objectManager_->GetCameraMatrix());
}
