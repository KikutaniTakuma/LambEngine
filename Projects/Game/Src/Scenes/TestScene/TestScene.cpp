#include "TestScene.h"
#include "Level/LevelLoader.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"
#include "Game/Water/Water.h"

#include "Utils/ScreenOut.h"

TestScene::TestScene():
	BaseScene(BaseScene::ID::Test)
{
}

void TestScene::Load()
{
	objectManager_ = ObjectManager::GetInstance();
	objectManager_->Load("./SceneData/TL2_Demo.json");
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
	Lamb::screenout << "TL2 Demo : TextureConverter" << Lamb::endline;

	objectManager_->Draw();

}
