#include "TestScene.h"
#include "Level/LevelLoader.h"

TestScene::TestScene():
	BaseScene(BaseScene::ID::Test)
{
}

void TestScene::Load()
{
	levelData_ = LevelLoader::Load("./SceneData/Test.json");
}

void TestScene::Initialize() {
	objectManager_ = ObjectManager::GetInstance();
	objectManager_->SetLevelData(levelData_);
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
