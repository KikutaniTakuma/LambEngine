#pragma once
#include "GameObject/Object.h"
#include <vector>
#include <memory>

/// <summary>
/// レベルデータ
/// </summary>
struct LevelData {
	std::vector<Lamb::SafePtr<Object>> objects;
	std::string name;
};