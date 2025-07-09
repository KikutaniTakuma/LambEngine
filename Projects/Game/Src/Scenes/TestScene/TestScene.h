#pragma once
#include "Scenes/Manager/BaseScene/BaseScene.h"
#include <chrono>


/// <summary>
/// テスト用
/// </summary>
class TestScene : public BaseScene {
private:
	enum class State {
		kReady,
		kCount,
		kStop
	};

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
	int Score(float sub);

private:
	std::chrono::steady_clock::time_point start_;
	std::chrono::steady_clock::time_point stop_;
	State state_;
	float time_;
	float threshold_;
	float thresholdTime_;
	float maxScore_;
	float middleScore_;
	float minScore_;
	float timeOverRange_;

	float topThresholdTime_;
};