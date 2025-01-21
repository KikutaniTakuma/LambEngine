#include "WaterComp.h"
#include "Game/Water/Water.h"
#include "../Comp/TransformComp.h"

void WaterComp::Init() {
	transformComp_ = object_.AddComp<TransformComp>();
}

void WaterComp::Update() {
	Water::GetInstance()->SetCameraPos(transformComp_->translate);
}

void WaterComp::Save(nlohmann::json& json) {
	SaveCompName(json);
}

void WaterComp::Load([[maybe_unused]]nlohmann::json& json) {
}
