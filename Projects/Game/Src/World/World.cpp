#include "World.h"
#include "Editor/ParticleEditor/ParticleEditor.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "../Game/Water/Water.h"
#include "GameObject/Manager/ObjectManager.h"
#include "GameObject/Manager/TransformCompUpdater.h"

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
		.fileName = "./Resources/EngineResources/Load.png",
		.animationNumber = 6,
		.animationSpeed = 0.1f
	};

	Framework::Initialize();
	Camera::InitStaticMatrix();

	Water::Initialize();


	StringOutPutManager::GetInstance()->LoadFont("./Resources/Font/default.spritefont");

	TransformCompUpdater::Initialize();
	ObjectManager::Initialize();

	// シーンマネージャー初期化
	SceneManager::Initialize();
	sceneManager_ = SceneManager::GetInstance();

	sceneManager_->Initialize(BaseScene::ID::Title, BaseScene::ID::Title);

	ParticleEditor::Initialize();
	particleEditor_ = ParticleEditor::GetInstance();

#ifndef USE_DEBUG_CODE
	Input::GetInstance()->GetMouse()->Show(false);
#endif // !USE_DEBUG_CODE

}

void World::Finalize() {
	Water::Finalize();

	if (sceneManager_) {
		sceneManager_->Finalize();
	}

	SceneManager::InstanceReset();

	ObjectManager::Finalize();
	TransformCompUpdater::Finalize();

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
		particleEditor_->Draw(sceneManager_->GetCurrentSceneCamera());
	}
	else {
		isEnd_ = true;
	}
}