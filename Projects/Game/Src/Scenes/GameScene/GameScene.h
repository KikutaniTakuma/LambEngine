#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Transform/Transform.h"
#include "Engine/Graphics/Skeleton.h"
#include "Engine/Graphics/Animator/Animator.h"
#include "Drawers/AnimationModel/AnimationModel.h"

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
	std::unique_ptr<AnimationModel> model_;
	Animations* animations_;
	Animations* animations2_;
	Lamb::SafePtr<Model> model2_;
	Transform transform_;
};