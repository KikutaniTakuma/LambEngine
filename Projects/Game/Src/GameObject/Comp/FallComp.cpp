#include "FallComp.h"

void FallComp::FirstUpdate() {
	if (isFall_) {
		fallTime_ += object_.GetDeltaTime();
	}
}

void FallComp::Start() {
	isFall_ = true;
}

void FallComp::Stop() {
	isFall_ = false;
	fallTime_ = 0.0_f32;
}
