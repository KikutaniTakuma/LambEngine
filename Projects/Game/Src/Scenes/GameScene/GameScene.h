#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Level/LevelLoader.h"

class GameScene : public BaseScene {
public:
	GameScene();
	GameScene(const GameScene&) = delete;
	GameScene(GameScene&&) = delete;
	~GameScene() = default;

	GameScene& operator=(const GameScene&) = delete;
	GameScene& operator=(GameScene&&) = delete;

public:
	void Load() override;

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

public:
	std::unique_ptr<LevelData> levelData_;
};