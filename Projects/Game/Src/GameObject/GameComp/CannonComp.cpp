#include "CannonComp.h"

void CannonComp::FirstUpdate() {
	if (isStart and not isFire_) {
		time_ += object_.GetDeltaTime();
	}

	if (not isFire_ and fireKeyTime_ <= time_) {
		isFire_ = true;
		velocity_ = strength_;
	}
}

void CannonComp::SetFireKeyTime(float32_t fireKeyTime) {
	fireKeyTime_ = fireKeyTime;
}

void CannonComp::SetStrength(float32_t strength) {
	strength_ = strength;
}
