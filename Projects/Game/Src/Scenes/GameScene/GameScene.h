#pragma once

#include "Scenes/Manager/SceneManager.h"
#include "Drawer/Model/Model.h"
#include "Drawer/Texture2D/Texture2D.h"
#include "Drawer/PeraRender/PeraRender.h"
#include "Drawer/Particle/Particle.h"
#include "Drawer/StringOut/StringOut.h"
#include "GlobalVariables/GlobalVariables.h"

#include "GameExternals/EditorManager/EditorManager.h"

#include "GameExternals/BlockManager/BlockManager.h"
#include "GameExternals/BulletManager/BulletManager.h"
#include "GameExternals/Customize/Customize.h"
#include "GameExternals/CollisionManager/CollisionManager.h"
#include "GameExternals/GoalManager/GoalManager.h"
#include "GameExternals/StartArea/StartArea.h"
#include "GameExternals/Player/Player.h"
#include "GameExternals/WindManager/WindManager.h"
#include "GameExternals/WhirlpoolManager/WhirlpoolManager.h"
#include "GameExternals/Cursor/Cursor.h"
#include "Game/Water/Water.h"
#include "Drawer/Other/WaterTex2D/WaterTex2D.h"

#include "Game/Corals/Corals.h"

#include "Game/Fishes/Fishes.h"

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

	Corals corals_;

	std::unique_ptr<Fishes> fishes_;

	Texture2D::Data tex2DData_;
};