#pragma once
#include "GameObject/Object.h"
#include <vector>
#include <memory>
struct LevelData {
	std::vector<Lamb::SafePtr<Object>> objects;
	std::string name;
};