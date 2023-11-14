#pragma once
#include "SceneManager/SceneManager.h"
#include "Lamb/Drawers/Model/Model.h"
#include "Lamb/Drawers/Texture2D/Texture2D.h"
#include "Lamb/Drawers/PeraRender/PeraRender.h"
#include "Lamb/Drawers/Particle/Particle.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/Player/Player.h"
#include "Game/MoveFloor/MoveFloor.h"
#include "Game/Goal/Goal.h"
#include "Game/Enemy/Enemy.h"

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
	Model model_;
	Model model2_;
	Texture2D tex2D_;
	Texture2D tex2D2_;

	Texture* texture_;
};