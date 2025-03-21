#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <bitset>
#include <optional>
#include "Fade/Fade.h"
#include "Input/Input.h"
#include "BaseScene/BaseScene.h"
#include "SceneLoad/SceneLoad.h"

/// <summary>
/// シーン管理
/// </summary>
class SceneManager final {
public:
	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	~SceneManager() = default;

	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

public:
	static void Initialize();
	static void InstanceReset();

	static SceneManager* const GetInstance();

private:
	static std::unique_ptr<SceneManager> pInstance_;
public:

	void Initialize(std::optional<BaseScene::ID> firstScene, std::optional<BaseScene::ID> finishID);
	void Finalize();

	/// <summary>
	/// シーン変更
	/// </summary>
	/// <param name="next">次のID</param>
	void SceneChange(std::optional<BaseScene::ID> next);

	void Update();

	void Draw();

public:
	inline bool GetIsPad() const {
		return isPad_;
	}

	bool IsEnd() const;

	const class Camera& GetCurrentSceneCamera() const;

	BaseScene::ID GetCurrentSceneID() const;
	BaseScene::ID GetPreSceneID() const;


private:
	void Debug();

	/// <summary>
	/// テクスチャをvramへ送る
	/// </summary>
	void UploadTextureData();


private:
	std::unique_ptr<BaseScene> scene_;
	std::unique_ptr<BaseScene> next_;

	std::unique_ptr<Fade> fade_;

	class FrameInfo* frameInfo_ = nullptr;
	Input* input_ = nullptr;

	bool isPad_ = false;

	std::optional<BaseScene::ID> finishID_;
	std::optional<BaseScene::ID> preSceneID_;

	std::unique_ptr<SceneLoad> load_;

#ifdef USE_DEBUG_CODE
	std::unordered_map<BaseScene::ID, std::string> sceneName_;
#endif // USE_DEBUG_CODE
	std::unordered_map<BaseScene::ID, uint8_t> sceneNum_;
};