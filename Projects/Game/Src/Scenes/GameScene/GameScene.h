#pragma once

#include "Scenes/Manager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"

#include "Editor/EditorManager/EditorManager.h"

#include "Game/BlockManager/BlockManager.h"
#include "Game/BulletManager/BulletManager.h"
#include "Game/Customize/Customize.h"
#include "Game/CollisionManager/CollisionManager.h"
#include "Game/GoalManager/GoalManager.h"
#include "Game/StartArea/StartArea.h"
#include "Game/Player/Player.h"
#include "Game/WindManager/WindManager.h"
#include "Game/WhirlpoolManager/WhirlpoolManager.h"
#include "Game/Water/Water.h"
#include "Game/Cursor/Cursor.h"
#include "Drawers/Other/WaterTex2D/WaterTex2D.h"

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
	std::unique_ptr<BlockManager> blockManager_;

	std::unique_ptr<CollisionManager> collisionManager_;

	std::unique_ptr<Customize> customize_;

	std::unique_ptr<EditorManager> editorManager_;
	
	std::unique_ptr<GoalManager> goalManager_;
	
	
	std::unique_ptr<Player> player_;

	
	std::unique_ptr<StartArea> startArea_;
	
	std::unique_ptr<WindManager> windManager_;
	
	std::unique_ptr<WhirlpoolManager> whirlpoolManager_;

	std::unique_ptr<Tex2DInstance> clearMessage_;

	std::unique_ptr<Camera> uiCamera_;

	std::unique_ptr<Cursor> cursor_;

	Water* water_;

	class Cloud* cloud_;

	bool isDebug_;

	class Audio* customizeBGM_ = nullptr;
	class Audio* seaSE_ = nullptr;

	WaterTex2D::WaveData waveData_;
};