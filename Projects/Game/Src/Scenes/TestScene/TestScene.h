#pragma once
#include "Scenes/Manager/BaseScene/BaseScene.h"
#include "GameObject/Manager/ObjectManager.h"
#include "Level/LevelData.h"
#include "Game/Fishes/Fishes.h"

#include "Game/WaterInteractive/WaterInteractive.h"

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
	
	class Water* water_;

	std::unique_ptr<Fishes> fishes_;

	std::unique_ptr<WaterInteractive> interactive_;
};