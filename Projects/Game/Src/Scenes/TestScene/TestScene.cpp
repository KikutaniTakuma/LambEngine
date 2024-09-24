#include "TestScene.h"
#include "Level/LevelLoader.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"

TestScene::TestScene():
	BaseScene(BaseScene::ID::Test)
{
}

void TestScene::Load()
{
	auto hoge = ShaderManager::GetInstance()->LoadMeshShader("./Resources/Shaders/MeshShader/Simple.MS.hlsl");
	hoge = ShaderManager::GetInstance()->LoadPixelShader("./Resources/Shaders/MeshShader/Simple.PS.hlsl");

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
