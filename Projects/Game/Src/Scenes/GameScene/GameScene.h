#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Transform/Transform.h"

class GameScene : public BaseScene {
public:
	GameScene();
	GameScene(const GameScene&) = delete;
	GameScene(GameScene&&) = delete;
	~GameScene() = default;

	GameScene& operator=(const GameScene&) = delete;
	GameScene& operator=(GameScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

public:
	Transform transform_;
	Transform uvTransform_;
	Texture2D* tex2D_;
};