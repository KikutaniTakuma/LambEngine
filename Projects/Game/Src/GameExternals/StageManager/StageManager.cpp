#include "StageManager.h"

uint32_t StageManager::currentStageIndex_ = 0;
std::array<bool, StageManager::kMaxStageCount> StageManager::isGoals_ = {false};