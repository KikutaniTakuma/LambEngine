#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Transform/Transform.h"
#include "Engine/Graphics/Skeleton.h"
#include "Engine/Graphics/Animator/Animator.h"

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
	std::unique_ptr<Skeleton> skeleton_;
	Lamb::SafePtr<Model> model_;
	std::unique_ptr<Animator> animator_;
	Transform transform_;
};