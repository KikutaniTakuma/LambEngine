#pragma once
#include "GameObject/Object.h"

#include <vector>
#include <memory>

struct LevelData {
	std::vector<std::unique_ptr<Object>> objects;
	std::string name;
};