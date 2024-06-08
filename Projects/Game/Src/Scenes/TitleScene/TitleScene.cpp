#include "TitleScene.h"
#include "AudioManager/AudioManager.h"
#include "GameObject/Comp/ModelRenderComp.h"

TitleScene::TitleScene() :
	BaseScene(BaseScene::ID::Game)
{}

void TitleScene::Load()
{
	levelData_.reset(LoadLevel("SceneData/TitleScene.json"));

	for (auto& i : levelData_->objects) {
		if (i->HasTag("Model")) {
			auto modelComp = i->GetComp<ModelRenderComp>();
			modelComp->Load();
		}
	}
}

void TitleScene::Initialize() {
	currentCamera_->farClip = 3000.0f;
	currentCamera_->pos.z = -10.0f;

	/*drawerManager_->LoadModel("./Resources/Common/Hololive/Watame/Watame.obj");
	model2_ = drawerManager_->GetModel("./Resources/Common/Hololive/Watame/Watame.obj");*/

	for (auto& i : levelData_->objects) {
		i->Init();
		i->SetCamera(currentCamera_.get());
	}
}

void TitleScene::Finalize() {

}

void TitleScene::Update() {
	currentCamera_->Debug("camera");
	currentCamera_->Update();

	for (auto& i : levelData_->objects) {
		i->Update();
	}
}

void TitleScene::Draw() {
	for (auto& i : levelData_->objects) {
		i->Draw();
	}
}