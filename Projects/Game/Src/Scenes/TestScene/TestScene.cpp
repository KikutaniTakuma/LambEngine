#include "TestScene.h"
#include "Level/LevelLoader.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"

TestScene::TestScene():
	BaseScene(BaseScene::ID::Test)
{
}

void TestScene::Load()
{
	objectManager_ = ObjectManager::GetInstance();
	objectManager_->Load("./SceneData/MeshShaderDrawTest.json");
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
