#pragma once
#include <unordered_map>
#include <variant>
#include <string>

#include "Drawer/Model/Model.h"
#include "Math/Vector3.h"

class Camera;
class WhirlpoolManager;
class WhirlpoolEditor {
public:
	WhirlpoolEditor();
	void Initialize();
	void Update();
	void Draw(const Camera& camera);

	void SetWhirlpoolManager(WhirlpoolManager* whirlpoolManager) { whirlpoolManager_ = whirlpoolManager; }
	void SaveFile(const std::string& stageName);
	void LoadFile(const std::string& stageName);
	void Clear();
private:

	WhirlpoolManager* whirlpoolManager_;
	Vector3 scale_;
	Vector3 rotate_;
	Vector3 position_;
	float gameOverSpeed_;

	std::string loadString_;
	std::string saveString_;
	std::string fileName_;

	int stageIndex_;
	std::unique_ptr<ModelInstance> model_;
	bool isCreate_;
};