#include "SceneFactory.h"
#include "Scenes/GameScene/GameScene.h"
#include "Scenes/TitleScene/TitleScene.h"
#include "Scenes/SelectStageScene/SelectStageScene.h"
#include "Scenes/TestScene/TestScene.h"

SceneFactory::SceneFactory():
	createScene_{}
{
	CreateFunctions_();
}

SceneFactory* const SceneFactory::GetInstance() {
	static SceneFactory instance{};
	return &instance;
}

BaseScene* SceneFactory::CreateBaseScene(std::optional<BaseScene::ID> createSceneID) {
	return createScene_[createSceneID]();
}

void SceneFactory::CreateFunctions_() {
	createScene_[BaseScene::ID::Game] =
		[]()->BaseScene* {
		return new GameScene();
		};
	createScene_[BaseScene::ID::Title] =
		[]()->BaseScene* {
		return new TitleScene();
		};
	createScene_[BaseScene::ID::Result] =
		[]()->BaseScene* {
		return nullptr;
		};
	createScene_[BaseScene::ID::StageSelect] =
		[]()->BaseScene* {
		return new SelectStageScene();
		};
	createScene_[BaseScene::ID::Test] =
		[]()->BaseScene* {
		return new TestScene();
		};
}