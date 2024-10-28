#pragma once
#include "Scenes/Manager/BaseScene/BaseScene.h"
#include "GameObject/Manager/ObjectManager.h"
#include "Level/LevelData.h"
#include "Game/Corals/Corals.h"

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

	Corals corals_;
	
	class Water* water_;
};