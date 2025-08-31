#pragma once
#include "Scenes/Manager/BaseScene/BaseScene.h"
#include <chrono>
#include <future>


/// <summary>
/// テスト用
/// </summary>
class TestScene : public BaseScene {
private:
	enum class State {
		kLogin,
		kNewUser,
		kReady,
		kCount,
		kStop,
		kCommunicating,
		kRanking
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

	std::future<std::string> PostScoreAsync(int score, const std::string& token);
	std::future<std::string> GetAllScoresAsync(const std::string& token);
	std::future<std::string> PostNewUserAsync(const std::string& name, const std::string& password);
	std::future<std::string> PostUserLoginAsync(const std::string& name, const std::string& password);

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

	int32_t score_;

	std::string rankingText_;


	std::string username_;
	std::string userPassword_;
	std::string token_;

	bool isLoginNow_ = false;
	bool isNewUser = false;

	char usernameBuf_[32];
	char userPasswordBuf_[32];
};