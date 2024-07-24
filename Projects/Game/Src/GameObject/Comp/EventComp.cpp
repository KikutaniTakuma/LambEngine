#include "EventComp.h"
#include "ObbComp.h"

void EventComp::Init() {
	obbComp = object_.AddComp<decltype(obbComp)::type>();
}

void EventComp::Event() {
	isEvent = obbComp->GetIsCollision();
}
