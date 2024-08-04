#pragma once

#include <string>

#include "Drawers/Model/Model.h"
#include "Math/Vector3.h"

class Camera;
class BlockManager;
class BlockEditor {
public:
	BlockEditor();
	void Initialize();
	void Update();
	void Draw(const Camera& camera);

	void SetBlockManager(BlockManager* blockManager) { blockManager_ = blockManager; }
	void SaveFile(const std::string& stageName);
	void LoadFile(const std::string& stageName);
	void Clear();
private:
	BlockManager* blockManager_;
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