#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/CollisionManager/Collider/Collider.h"
#include "Utils/Easeing.h"
#include "Game/SkyDome/SkyDome.h"
#include "Game/CollisionManager/Sphere/Sphere.h"
#include "Engine/Graphics/Tex2DAniamtor/Tex2DAniamtor.h"
#include "Utils/SafePtr.h"
#include "Drawers/Other/WaterTex2D/WaterTex2D.h"
#include "Game/SkyBlock/SkyBlock.h"
#include "Game/Player/Player.h"
#include "Game/Coin/Coin.h"

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
	class Water* water_;
	Lamb::SafePtr<LevelData> levelData_;
	//std::unique_ptr<Coin> coin_;
};