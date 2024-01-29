#include "World.h"
#include "Scenes/Manager/SceneManager.h"
#include "Editor/ParticleEditor/ParticleEditor.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "../Game/Water/Water.h"
#include "../Game/Cloud/Cloud.h"

void World::Initialize() {
	// ウィンドウ初期化オプション
	initDesc_ = Framework::InitDesc{
		.windowName = "LambEngine_demo",
		.windowSize = {1280.0f, 720.0f},
		.maxFps = 60.0f,
		.isFullesceen = false
	};

	// ロードのステータス
	SceneLoad::setting = SceneLoad::Desc{
		.fileName = "./Resources/Load.png",
		.animationNumber = 4,
		.animationSpeed = 500
	};

	Framework::Initialize();

	Water::Initialize();

	Cloud::Initialize();

	StringOutPutManager::GetInstance()->LoadFont("./Resources/Font/mincho_size_32.spritefont");



	sceneManager_ = SceneManager::GetInstance();

	sceneManager_->Initialize(BaseScene::ID::Title, BaseScene::ID::Title);

	ParticleEditor::Initialize();
	particleEditor_ = ParticleEditor::GetInstance();
}

void World::Finalize() {
	Cloud::Finalize();
	Water::Finalize();

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