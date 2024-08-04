#pragma once

#include <array>
#include <string>

class StageManager {
public:
	static const uint32_t kMaxStageCount = 12;
	static const uint32_t kCanSailStageCount = 5;
	static const uint32_t kCanLoopCannonStageCount = 9;
	static void SetStage(uint32_t stage) { currentStageIndex_ = stage; }
	static uint32_t GetStage() { return currentStageIndex_; }
	static std::string GetCurrentStageString() { return "Stage_" + std::to_string(currentStageIndex_ + 1); }
	static std::string GetStageString(uint32_t index) { return "Stage_" + std::to_string(index + 1); }

	static std::string GetStageDataString() { return "StageDatas"; }

	static void SetGaol(bool flag) {
		isGoals_[currentStageIndex_] = flag;
	}

	static bool GetIsGoal(uint32_t stageNumber) {
		return isGoals_[stageNumber];
	}

	static void ResetIsGoal() {
		isGoals_ = { false };
	}

private:
	// ステージ１の時currentStageIndex_は0なので気を付けて
	static uint32_t currentStageIndex_;

	static std::array<bool, kMaxStageCount> isGoals_;
};