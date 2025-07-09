#include "TestScene.h"
#include "Level/LevelLoader.h"
#include "Engine/Graphics/Shader/ShaderManager/ShaderManager.h"
#include "Game/Water/Water.h"

#include "Utils/ScreenOut.h"

#include "Utils/Easing.h"

TestScene::TestScene():
	BaseScene(BaseScene::ID::Test),
	start_(),
	stop_(),
	state_(State::kReady),
	time_(0.0f),
	threshold_(7.0f),
	thresholdTime_(10.0f),
	maxScore_(1000000.0f),
	middleScore_(10000.0f),
	minScore_(0.0f),
	timeOverRange_(0.05f),
	topThresholdTime_(0.5f)
{
}

void TestScene::Load()
{
}

void TestScene::Initialize() {
}

void TestScene::Finalize() {
}

void TestScene::Update()
{
	switch (state_)
	{
	case TestScene::State::kReady:
		if (input_->GetKey()->Pushed(DIK_SPACE)) {
			start_ = std::chrono::steady_clock::now();
			state_ = State::kCount;
		}
		break;
	case TestScene::State::kCount:
		if (input_->GetKey()->Pushed(DIK_SPACE)) {
			stop_ = std::chrono::steady_clock::now();
			state_ = State::kStop;
		}
		break;
	case TestScene::State::kStop:
		if (input_->GetKey()->Pushed(DIK_SPACE)) {
			state_ = State::kReady;
			time_ = 0.0f;
		}
		break;
	default:
		break;
	}

	std::chrono::steady_clock::time_point now = {};
	int64_t duration = 0;
	float sub = 0.0f;
	int score = 0;


	switch (state_)
	{
	case TestScene::State::kReady:
		Lamb::screenout << "time count ready";
		break;
	case TestScene::State::kCount:
		now = std::chrono::steady_clock::now();

		duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();

		time_ = static_cast<float>(duration) * 0.001f;

		if (time_ < threshold_) {
			Lamb::screenout << "time : " << time_ << "s";
		}
		else {
			Lamb::screenout << "time : " << "???s";
		}
		break;
	case TestScene::State::kStop:
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_ - start_).count();

		time_ = static_cast<float>(duration) * 0.001f;
		
		sub = thresholdTime_ - time_;
		
		score = Score(sub);

		Lamb::screenout << "result time : " << time_ << "s" << Lamb::endline
			<< "score : " << score;

		break;
	default:
		break;
	}
}

void TestScene::Draw()
{
	
}

int TestScene::Score(float sub)
{
	float score = 0.0f;

	sub = thresholdTime_ - time_;
	// 10秒超過かつ誤差0.05秒以上
	if (std::signbit(sub) and (timeOverRange_ < std::abs(sub))) {
		score = 0.0f;
	}
	// 10秒により近い
	else if (std::abs(sub) < topThresholdTime_) {
		score = std::lerp(maxScore_, middleScore_, Easing::OutCirc(std::abs(sub) / topThresholdTime_));
	}
	// それ以外
	else {
		score = std::lerp(middleScore_, minScore_, Easing::InOutExpo(sub / (thresholdTime_ - topThresholdTime_)));
	}

	return static_cast<int>(score);
}
