#include "Engine/Engine.h"
#include "Engine/ErrorCheck/ErrorCheck.h"

#include "SceneManager/GameScene/GameScene.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// ライブラリ初期化
	if (!Engine::Initialize("DirectXGame", Vector2{ 1280.0f, 720.0f })) {
		ErrorCheck::GetInstance()->ErrorTextBox(
			"Engine::Initialize() Failed",
			"WinMain"
		);
		return -1;
	}
	// シーンマネージャー初期化
	auto* const sceneManager = SceneManager::GetInstace();
	auto scene = new GameScene{};
	
	sceneManager->Initialize(scene);

	// メインループ
	sceneManager->Game(BaseScene::ID::Game);

	sceneManager->Finalize();

	// ライブラリ終了
	Engine::Finalize();

	return 0;
}