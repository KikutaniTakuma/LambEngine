#include "SceneChangeComp.h"
#include "Scenes/Manager/SceneManager.h"

void SceneChangeComp::Init() {
	EventComp::Init();
}

void SceneChangeComp::Event() {
	EventComp::Event();

	if (isEvent) {
		sceneManager_->SceneChange(nextID_);
	}
}

void SceneChangeComp::SetNextScene(BaseScene::ID nextID) {
	nextID_ = nextID;
}

void SceneChangeComp::SetSceneManager(SceneManager* sceneManager)
{
	sceneManager_ = sceneManager;
}

void SceneChangeComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
	json["nextID"] = BaseScene::kSceneStrings[size_t(nextID_)];
}

void SceneChangeComp::Load(nlohmann::json& json)
{
	std::string&& id = json["nextID"].get<std::string>();
	for (size_t count = 0; BaseScene::kSceneStrings.size(); count++) {
		if (id == BaseScene::kSceneStrings[count]) {
			nextID_ = BaseScene::ID(count);
			break;
		}
	}
}
