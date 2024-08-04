#include "SailComp.h"
#include "../Comp/ObbComp.h"

void SailComp::Init() {
	ItemComp::Init();

	obbComp_ = object_.AddComp<ObbComp>();
}

void SailComp::Event() {
	if (isStart_ and not isOpen_) {
		time_ += object_.GetDeltaTime();
	}

	if (openKeyTime_ <= time_) {
		isOpen_ = true;
	}

	if (isOpen_ and obbComp_->GetIsCollision()) {
		velocity_ = strength_;
	}
	else {
		velocity_ = 0.0f;
	}
}

void SailComp::SetOpenKeyTime(float32_t openKeyTime)
{
	openKeyTime_ = openKeyTime;
}

void SailComp::SetStrength(float32_t strength) {
	strength_ = strength;
}

void SailComp::Save(nlohmann::json& json) {
	SaveCompName(json);
	json["openKeyTime"] = openKeyTime_;
	json["strength"] = strength_;
}

void SailComp::Load(nlohmann::json& json)
{
	openKeyTime_ = json["openKeyTime"].get<float32_t>();
	strength_ = json["strength"].get<float32_t>();
}
