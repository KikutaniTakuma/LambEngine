#pragma once
#include "Utils/Camera/Camera.h"
#include <memory>

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

	class MeshManager* meshManager_;

	class AudioManager* audioManager_;

	class TextureManager* textureManager_;

	class FrameInfo* frameInfo_;

	class Input* input_;

	class StringOutPutManager* stringOutPutManager_;

	BaseScene::ID sceneID_;

protected:
	std::unique_ptr<Camera> camera_;
};