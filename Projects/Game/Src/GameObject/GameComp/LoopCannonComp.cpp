#include "LoopCannonComp.h"

void LoopCannonComp::FirstUpdate() {
	if (isStart) {
		time_ += object_.GetDeltaTime();
	}

	if (fireKeyTime_ <= time_) {
		time_ = 0.0f;
		velocity_ = strength_;
	}
}

void LoopCannonComp::SetFireKeyTime(float32_t fireKeyTime) {
	fireKeyTime_ = fireKeyTime;
}

void LoopCannonComp::SetStrength(float32_t strength) {
	strength_ = strength;
}
