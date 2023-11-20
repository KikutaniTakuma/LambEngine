#include "World.h"
#include "Scenes/Manager/SceneManager.h"

void World::Initialize() {
	Framework::Initialize();

	sceneManager_ = SceneManager::GetInstance();

	sceneManager_->Initialize(BaseScene::ID::Game, BaseScene::ID::Game);
}

void World::Finalize() {
	if (sceneManager_) {
		sceneManager_->Finalize();
	}

	Framework::Finalize();
}

void World::Update() {
	if (sceneManager_) {
		sceneManager_->Update();
		isEnd_ = sceneManager_->IsEnd();
	}
	else {
		isEnd_ = true;
	}
}

void World::Draw() {
	if (sceneManager_) {
		sceneManager_->Draw();
	}
	else {
		isEnd_ = true;
	}
}