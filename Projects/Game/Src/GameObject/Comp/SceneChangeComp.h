#pragma once
#include "EventComp.h"
#include "Scenes/Manager/BaseScene/BaseScene.h"


class SceneChangeComp : public EventComp {
public:
	~SceneChangeComp() = default;

public:
	void Init() override;

	void Event() override;

	void SetNextScene(BaseScene::ID nextID);
	void SetSceneManager(class SceneManager* sceneManager);

private:
	BaseScene::ID nextID_ = BaseScene::ID::Title;
	Lamb::SafePtr<class SceneManager> sceneManager_;
};