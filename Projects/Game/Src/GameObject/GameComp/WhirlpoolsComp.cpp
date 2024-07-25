#include "WhirlpoolsComp.h"
#include "../Comp/ObbPushComp.h"

void WhirlpoolsComp::Init() {
	obbComp_ = object_.AddComp<ObbPushComp>();
}

bool WhirlpoolsComp::CanPathThrough(float32_t speed)
{
	return pathThroughSpeed_ <= speed;
}
