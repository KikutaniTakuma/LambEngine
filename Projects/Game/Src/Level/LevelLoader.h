#pragma once
#include "LevelData.h"
#include "Transform/Transform.h"

#include "Utils/FileUtils.h"

class LevelLoader {
private:
	LevelLoader() = delete;
	LevelLoader(const LevelLoader&&) = delete;
	LevelLoader(LevelLoader&&) = delete;
	~LevelLoader() = delete;
public:
	static [[nodiscard]] LevelData* Load(const std::string& fileName);

private:
	static void AddObjects(nlohmann::json& data, Lamb::SafePtr<LevelData> levelData);
	static void AddTransform(nlohmann::json& data, Object& object);
	static void AddCamera(nlohmann::json& data, Object& object);
	static void AddObb(nlohmann::json& data, Object& object);
	static void AddChildren(nlohmann::json& data, Lamb::SafePtr<LevelData> levelData, Object& parent);
};