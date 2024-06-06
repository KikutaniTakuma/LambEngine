#include "GameScene.h"
#include "AudioManager/AudioManager.h"
#include "GameObject/Comp/ModelRenderComp.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Load()
{
	levelData_.reset(LoadLevel("SceneData/GameScene.json"));

	for (auto& i : levelData_->objects) {
		if (i->HasTag("Model")) {
			auto modelComp = i->GetComp<ModelRenderComp>();
			modelComp->Load();
		}
	}
}

void GameScene::Initialize() {
	currentCamera_->farClip = 3000.0f;
	currentCamera_->pos.z = -10.0f;
	
	/*drawerManager_->LoadModel("./Resources/Common/Hololive/Watame/Watame.obj");
	model2_ = drawerManager_->GetModel("./Resources/Common/Hololive/Watame/Watame.obj");*/
	
	for (auto& i : levelData_->objects) {
		i->Init();
		i->SetCamera(currentCamera_.get());
	}
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	currentCamera_->Debug("camera");
	currentCamera_->Update();

	for (auto& i : levelData_->objects) {
		i->Update();
	}
}

void GameScene::Draw() {
	for (auto& i : levelData_->objects) {
		i->Draw();
	}
}