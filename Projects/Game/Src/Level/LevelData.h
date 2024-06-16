#pragma once
#include "GameObject/Object.h"
#include <vector>
#include <memory>

#include "Game/Player/Player.h"
#include "Game/SkyBlock/SkyBlock.h"

struct LevelData {
	std::vector<std::unique_ptr<Object>> objects;
	std::string name;
	std::vector<std::unique_ptr<SkyBlock>> skyBlocks;
	std::unique_ptr<Player> player;
};