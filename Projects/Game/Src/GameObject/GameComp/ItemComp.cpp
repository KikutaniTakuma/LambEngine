#include "ItemComp.h"

void ItemComp::Start() {
	isStart = true;
}

float32_t ItemComp::GetVelocity() const
{
	return velocity_;
}
