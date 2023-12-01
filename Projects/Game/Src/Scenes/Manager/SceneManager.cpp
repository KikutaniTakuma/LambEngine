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
		fade_->InStart();
		scene_->Finalize();
		scene_.reset(next_.release());
		next_.reset();
		scene_->Initialize();
		scene_->Update();
		fade_->Update();
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
	fade_.reset();
	if (scene_) {
		scene_->Finalize();
	}
	scene_.reset();
	if (next_) {
		next_->Finalize();
	}
	next_.reset();
}
