#include "TestScene.h"
#include "Level/LevelLoader.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"
#include "Game/Water/Water.h"

TestScene::TestScene():
	BaseScene(BaseScene::ID::Test)
{
	water_ = Water::GetInstance();
}

void TestScene::Load()
{
	objectManager_ = ObjectManager::GetInstance();
	objectManager_->Load("./SceneData/waterTest.json");
}

void TestScene::Initialize() {
}

void TestScene::Finalize() {

}

void TestScene::Update()
{
	objectManager_->Update();

	water_->Update();
}

void TestScene::Draw()
{
	water_->Draw(objectManager_->GetCameraMatrix());
	objectManager_->Draw();
}
