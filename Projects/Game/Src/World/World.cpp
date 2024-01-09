#include "World.h"
#include "Scenes/Manager/SceneManager.h"
#include "Editor/ParticleEditor/ParticleEditor.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "../Game/Water/Water.h"

void World::Initialize() {
	// ウィンドウ初期化オプション
	initDesc_ = Framework::InitDesc{
		.windowName = "LE2A_06_キクタニ_タクマ_水面",
		.windowSize = {1280.0f, 720.0f},
		.maxFps = 60.0f,
#ifdef _DEBUG
		.isFullesceen = false
#else
		.isFullesceen = true
#endif // _DEBUG
	};

	Framework::Initialize();

	Water::Initialize();
	sceneManager_ = SceneManager::GetInstance();

	sceneManager_->Initialize(BaseScene::ID::Title, BaseScene::ID::Title);

	ParticleEditor::Initialize();
	particleEditor_ = ParticleEditor::GetInstance();

	StringOutPutManager::GetInstance()->LoadFont("./Resources/Font/mincho_size_32.spritefont");

}

void World::Finalize() {
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