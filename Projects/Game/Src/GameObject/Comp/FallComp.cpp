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

void FallComp::Save(nlohmann::json& json) {
	SaveCompName(json);
	json["gravity"] = gravity;
}

void FallComp::Load(nlohmann::json& json)
{
	gravity = json["gravity"].get<float32_t>();
}
