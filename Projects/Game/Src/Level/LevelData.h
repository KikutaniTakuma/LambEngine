#pragma once
#include "GameObject/Object.h"
#include <vector>
#include <memory>

#include "Game/Player/Player.h"
#include "Game/SkyBlock/SkyBlock.h"

struct LevelData {
	std::vector<Lamb::SafePtr<Object>> objects;
	std::string name;
};