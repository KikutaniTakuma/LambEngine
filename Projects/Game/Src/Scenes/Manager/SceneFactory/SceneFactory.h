#pragma once
#include <optional>
#include <unordered_map>
#include <functional>
#include "Scenes/Manager/BaseScene/BaseScene.h"

/// <summary>
/// シーン生成
/// </summary>
class SceneFactory final {
private:
	SceneFactory();
	SceneFactory(const SceneFactory&) = delete;
	SceneFactory(SceneFactory&&) = delete;
	~SceneFactory() = default;

	SceneFactory& operator=(const SceneFactory&) = delete;
	SceneFactory& operator=(SceneFactory&&) = delete;

public:
	static SceneFactory* const GetInstance();

public:
	/// <summary>
	/// シーン作成
	/// </summary>
	/// <param name="createSceneID">シーンID</param>
	/// <returns></returns>
	BaseScene* CreateBaseScene(std::optional<BaseScene::ID> createSceneID);

private:
	/// <summary>
	/// 作成する関数を作成
	/// </summary>
	void CreateFunctions_();

private:
	std::unordered_map<std::optional<BaseScene::ID>, std::function<BaseScene* (void)>> createScene_;
};