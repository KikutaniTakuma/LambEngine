#pragma once
#include "LevelData.h"
#include "Transform/Transform.h"

#include "Utils/FileUtils.h"

/// <summary>
/// レベルローダー
/// </summary>
class LevelLoader {
private:
	LevelLoader() = delete;
	LevelLoader(const LevelLoader&&) = delete;
	LevelLoader(LevelLoader&&) = delete;
	~LevelLoader() = delete;
public:
	static [[nodiscard]] LevelData* Load(const std::string& fileName);

private:
	/// <summary>
	/// オブジェクト追加
	/// </summary>
	static void AddObjects_(nlohmann::json& data, Lamb::SafePtr<LevelData> levelData);
	/// <summary>
	/// トランスフォーム追加
	/// </summary>
	static void AddTransform_(nlohmann::json& data, Object& object);
	/// <summary>
	/// カメラ追加
	/// </summary>
	static void AddCamera_(nlohmann::json& data, Object& object);
	/// <summary>
	/// obb追加
	/// </summary>
	static void AddObb_(nlohmann::json& data, Object& object);
	/// <summary>
	/// children追加
	/// </summary>
	static void AddChildren_(nlohmann::json& data, Lamb::SafePtr<LevelData> levelData, Object& parent);
};