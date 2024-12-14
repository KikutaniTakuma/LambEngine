#pragma once
#include "Scenes/Manager/BaseScene/BaseScene.h"
#include "GameObject/Manager/ObjectManager.h"
#include "Level/LevelData.h"
#include "Game/Fishes/Fishes.h"

/// <summary>
/// テスト用
/// </summary>
class TestScene : public BaseScene {
public:
	TestScene();
	~TestScene() = default;

public:
	void Load();

	void Initialize();
	void Finalize();

	void Update();
	void Draw();

private:
	Lamb::SafePtr<ObjectManager> objectManager_;
	Lamb::SafePtr<LevelData> levelData_;
	

};