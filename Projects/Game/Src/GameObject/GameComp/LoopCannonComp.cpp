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

void LoopCannonComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
	json["fireKeyTime"] = fireKeyTime_;
	json["strength"] = strength_;
}

void LoopCannonComp::Load(nlohmann::json& json)
{
	fireKeyTime_ = json["fireKeyTime"].get<float32_t>();
	strength_ = json["strength"].get<float32_t>();
}
