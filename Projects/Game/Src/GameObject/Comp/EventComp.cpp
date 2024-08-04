#include "EventComp.h"
#include "ObbComp.h"

void EventComp::Init() {
	obbComp = object_.AddComp<decltype(obbComp)::type>();
}

void EventComp::Event() {
	isEvent = obbComp->GetIsCollision();
}

void EventComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
}

void EventComp::Load([[maybe_unused]]nlohmann::json& json)
{
}
