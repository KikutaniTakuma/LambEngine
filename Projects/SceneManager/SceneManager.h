#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <bitset>
#include <optional>
#include "Alfort/Utils/Camera/Camera.h"
#include "Fade/Fade.h"
#include "Alfort/MeshManager/MeshManager.h"
#include "Alfort/TextureManager/TextureManager.h"
#include "Alfort/AudioManager/AudioManager.h"
#include "Alfort/Engine/FrameInfo/FrameInfo.h"
#include "Alfort/Input/Input.h"

class BaseScene {
	friend class SceneManager;

public:
	enum class ID {
		Game,
		Result,
		Title,
		StageSelect,
	};

public:
	BaseScene(BaseScene::ID sceneID);
	BaseScene(const BaseScene&) = delete;
	BaseScene(BaseScene&&) = delete;
	virtual ~BaseScene() = default;

	BaseScene& operator=(const BaseScene&) = delete;
	BaseScene& operator=(BaseScene&&) = delete;

public:
	void SceneInitialize(class SceneManager* sceneManager);

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	virtual void Update() = 0;
	virtual void Draw() = 0;

	inline BaseScene::ID GetID() const {
		return sceneID_;
	}

protected:
	class SceneManager* sceneManager_;

	MeshManager* meshManager_;

	AudioManager* audioManager_;

	TextureManager* textureManager_;

	FrameInfo* frameInfo_;

	Input* input_;

	BaseScene::ID sceneID_;

protected:
	Camera camera_;
};

class SceneManager final {
private:
	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	~SceneManager() = default;

	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

public:
	static SceneManager* const GetInstace();

public:
	void Initialize(BaseScene* firstScene);
	void Finalize();

	void SceneChange(BaseScene* next);

	void Update();

	void Draw();

	void Game(std::optional<BaseScene::ID> finishID = BaseScene::ID::Title);

public:
	inline bool GetIsPad() const {
		return isPad_;
	}

private:
	std::unique_ptr<BaseScene> scene_;
	std::unique_ptr<BaseScene> next_;

	std::unique_ptr<Fade> fade_;

	Camera fadeCamera_;

	FrameInfo* frameInfo_ = nullptr;
	Input* input_ = nullptr;

	bool isPad_ = false;
};