#pragma once
#include <memory>

#include "GameExternals/BlockEditor/BlockEditor.h"
#include "GameExternals/GoalEditor/GoalEditor.h"
#include "GameExternals/WhirlpoolEditor/WhirlpoolEditor.h"
#include "GameExternals/WindEditor/WindEditor.h"

class BlockManager;
class GoalManager;
class WindManager;
class WhirlpoolManager;
class Camera;
class EditorManager {
public:
	void Initialize(BlockManager* blockManager, GoalManager* goalManager, WindManager* windManager, WhirlpoolManager* whirlpoolManager);

	void Update();

	void Draw(const Camera& camera);
private:
	std::unique_ptr<BlockEditor> blockEditor_;
	std::unique_ptr<GoalEditor> goalEditor_;
	std::unique_ptr<WhirlpoolEditor> whirlpoolEditor_;
	std::unique_ptr<WindEditor> windEditor_;

	int stageIndex_;
	std::string loadString_;
	std::string saveString_;
};