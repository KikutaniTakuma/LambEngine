#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
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

private:
	void CreateRockOnPosisions();

private:
	void InitEnemy();

private:
	static constexpr int32_t kEnemyNumber_ = 5;

public:
	std::vector<Model> models_;
	std::vector<Texture2D> texs_;

	std::unique_ptr<Player> player_;
	std::unique_ptr<Goal> goal_;
	std::list<std::unique_ptr<Enemy>> enemys_;
	std::unique_ptr<Model> skyDome_;

	std::vector<MoveFloor> floor_;

	GlobalVariables globalVariables_;

	std::unique_ptr<class RockOn> rockOn_;
	std::unique_ptr<class FollowCamera> followCamera_;

	std::vector<Vector3> enemyPossiotns_;
};