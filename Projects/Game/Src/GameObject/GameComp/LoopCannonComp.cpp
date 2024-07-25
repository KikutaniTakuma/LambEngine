#include "LoopCannonComp.h"

void LoopCannonComp::Event() {
	if (isStart_) {
		time_ += object_.GetDeltaTime();
	}

	if (fireKeyTime_ <= time_) {
		time_ = 0.0f;
		velocity_ = strength_;
	}
	else {
		velocity_ = 0.0f;
	}
}

void LoopCannonComp::SetFireKeyTime(float32_t fireKeyTime) {
	fireKeyTime_ = fireKeyTime;
}

void LoopCannonComp::SetStrength(float32_t strength) {
	strength_ = strength;
}
