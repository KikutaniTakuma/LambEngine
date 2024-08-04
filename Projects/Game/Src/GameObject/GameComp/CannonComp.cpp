#include "CannonComp.h"

void CannonComp::Event() {
	if (isStart_ and not isFire_) {
		time_ += object_.GetDeltaTime();
	}

	if (not isFire_ and fireKeyTime_ <= time_) {
		isFire_ = true;
		velocity_ = strength_;
	}
	else {
		velocity_ = 0.0f;
	}
}

void CannonComp::SetFireKeyTime(float32_t fireKeyTime) {
	fireKeyTime_ = fireKeyTime;
}

void CannonComp::SetStrength(float32_t strength) {
	strength_ = strength;
}

void CannonComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
	json["fireKeyTime"] = fireKeyTime_;
	json["strength"] = strength_;
}

void CannonComp::Load(nlohmann::json& json)
{
	fireKeyTime_ = json["fireKeyTime"].get<float32_t>();
	strength_ = json["strength"].get<float32_t>();
}
