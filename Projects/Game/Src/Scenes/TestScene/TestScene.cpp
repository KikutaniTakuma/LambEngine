#include "TestScene.h"
#include "Level/LevelLoader.h"

TestScene::TestScene():
	BaseScene(BaseScene::ID::Test)
{
}

void TestScene::Load()
{
	objectManager_ = ObjectManager::GetInstance();
	objectManager_->Load("./SceneData/Test.json");
}

void TestScene::Initialize() {
}

void TestScene::Finalize() {

}

void TestScene::Update()
{
	objectManager_->Update();
}

void TestScene::Draw()
{
	objectManager_->Draw();
}
