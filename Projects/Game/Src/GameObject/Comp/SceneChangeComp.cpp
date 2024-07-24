#include "SceneChangeComp.h"
#include "Scenes/Manager/SceneManager.h"

void SceneChangeComp::Init() {
	EventComp::Init();
}

void SceneChangeComp::Event() {
	EventComp::Event();

	if (isEvent) {
		sceneManager_->SceneChange(nextID_);
	}
}

void SceneChangeComp::SetNextScene(BaseScene::ID nextID) {
	nextID_ = nextID;
}

void SceneChangeComp::SetSceneManager(SceneManager* sceneManager)
{
	sceneManager_ = sceneManager;
}
