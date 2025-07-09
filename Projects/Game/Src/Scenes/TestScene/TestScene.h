#pragma once
#include "Scenes/Manager/BaseScene/BaseScene.h"


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
	
};