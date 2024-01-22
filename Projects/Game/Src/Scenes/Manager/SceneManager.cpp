#include "SceneManager.h"
#include "Engine/Engine.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include <filesystem>
#include <fstream>
#include <format>
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Input/Input.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "SceneFactory/SceneFactory.h"
#include "Engine/Graphics/ResourceManager/ResourceManager.h"

SceneManager* const SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Initialize(std::optional<BaseScene::ID> firstScene, std::optional<BaseScene::ID> finishID) {
	fade_ = std::make_unique<Fade>();
	fadeCamera_.Update();

	frameInfo_ = FrameInfo::GetInstance();
	input_ = Input::GetInstance();

	SceneFactory* const sceneFactory = SceneFactory::GetInstance();

	scene_.reset(sceneFactory->CreateBaseScene(firstScene));
	scene_->SceneInitialize(this);
	scene_->Initialize();

	StringOutPutManager::GetInstance()->LoadFont("./Resources/Font/default.spritefont");

	finishID_ = finishID;

	load_.reset(new SceneLoad{});

	ResourceManager::GetInstance()->Enable();
}

void SceneManager::SceneChange(std::optional<BaseScene::ID> next) {
	if (next_) {
		return;
	}
	SceneFactory* const sceneFactory = SceneFactory::GetInstance();

	next_.reset(sceneFactory->CreateBaseScene(next));
	next_->SceneInitialize(this);

	fade_->OutStart();
}

void SceneManager::Update() {
	if (input_->GetGamepad()->PushAnyKey()) {
		isPad_ = true;
	}
	else if (input_->GetMouse()->PushAnyKey() || input_->GetKey()->PushAnyKey()) {
		isPad_ = false;
	}

	if (scene_ && !next_ && !fade_->InStay()) {
		scene_->Update();
	}
	else {
		fade_->Update();
	}

	if (fade_->OutEnd()) {
		// ロード中の描画を開始
		load_->Start();

#pragma region シーン切り替え
		// シーン終わり処理
		scene_->Finalize();
		// 次のシーンへ
		scene_.reset(next_.release());
		// 次のシーンを格納するものユニークポインタをリセット
		next_.reset();

		ResourceManager::GetInstance()->Unload();
#pragma endregion

#pragma region ロード中
		// シーンの初期化
		scene_->Initialize();
		// ロード中の描画を終了
		load_->Stop();
#pragma endregion

#pragma region その後の処理
		// フェードスタート
		fade_->InStart();

		// シーンの更新処理
		scene_->Update();

		// フェードの更新処理
		fade_->Update();
#pragma endregion
	}
	else if (fade_->InEnd()) {
		fade_->Update();
	}
}

void SceneManager::Draw() {
	if (scene_) {
		scene_->Draw();
	}

	fade_->Draw(fadeCamera_.GetViewOthographics());
}

bool SceneManager::IsEnd() const {
	if (!scene_) {
		return true;
	}

	return scene_->GetID() == finishID_ && 
	(input_->GetKey()->Pushed(DIK_ESCAPE) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::START));
}

void SceneManager::Finalize() {
	if (load_) {
		load_.reset();
	}


	fade_.reset();
	if (scene_) {
		scene_->Finalize();
	}
	scene_.reset();
	if (next_) {
		next_->Finalize();
	}
	next_.reset();

	ResourceManager::GetInstance()->Unenable();
}
