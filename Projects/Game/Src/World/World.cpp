#include "World.h"
#include "Scenes/Manager/SceneManager.h"
#include "Editor/ParticleEditor/ParticleEditor.h"

void World::Initialize() {
	// ウィンドウ初期化オプション
	initDesc_ = Framework::InitDesc{
		.windowName = "LambEngine_demo",
		.windowSize = {1280.0f, 720.0f},
		.maxFps = 60.0f,
		.isFullesceen = false
	};

	Framework::Initialize();

	sceneManager_ = SceneManager::GetInstance();

	sceneManager_->Initialize(BaseScene::ID::Game, BaseScene::ID::Game);

	ParticleEditor::Initialize();
	particleEditor_ = ParticleEditor::GetInstance();
}

void World::Finalize() {
	if (sceneManager_) {
		sceneManager_->Finalize();
	}

	ParticleEditor::Finalize();

	Framework::Finalize();
}

void World::Update() {
	if (sceneManager_) {
		sceneManager_->Update();
		particleEditor_->Editor();
		isEnd_ = sceneManager_->IsEnd();
	}
	else {
		isEnd_ = true;
	}
}

void World::Draw() {
	if (sceneManager_) {
		sceneManager_->Draw();
		particleEditor_->Draw();
	}
	else {
		isEnd_ = true;
	}
}