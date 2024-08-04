#pragma once

#include <string>

#include "Drawers/Model/Model.h"
#include "Math/Vector3.h"

class Camera;
class GoalManager;
class GoalEditor {
public:
	GoalEditor();
	void Initialize();
	void Update();
	void Draw(const Camera& camera);

	void SetGoalManager(GoalManager* goalManager) { goalManager_ = goalManager; }
	void SaveFile(const std::string& stageName);
	void LoadFile(const std::string& stageName);
	void Clear();
private:

	GoalManager* goalManager_;
	Vector3 scale_;
	Vector3 rotate_;
	Vector3 position_;

	std::string loadString_;
	std::string saveString_;
	std::string fileName_;

	int stageIndex_;

	std::unique_ptr<ModelInstance> model_;
	bool isCreate_;
};