#pragma once

#include <string>

#include "Drawer/Model/Model.h"
#include "Math/Vector3.h"

class Camera;
class WindManager;
class WindEditor {
public:
	WindEditor();
	void Initialize();
	void Update();
	void Draw(const Camera& camera);

	void SetWindManager(WindManager* windManager) { windManager_ = windManager; }
	void SaveFile(const std::string& stageName);
	void LoadFile(const std::string& stageName);
	void Clear();
private:

	WindManager* windManager_;
	Vector3 scale_;
	Vector3 rotate_;
	Vector3 position_;
	Vector3 vector_;

	std::string loadString_;
	std::string saveString_;
	std::string fileName_;

	int stageIndex_;

	std::unique_ptr<ModelInstance> model_;
	bool isCreate_;
};