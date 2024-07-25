#pragma once
#include "ItemComp.h"
class LoopCannonComp : public ItemComp {
public:
	~LoopCannonComp() = default;

public:
	void Event() override;

	void SetFireKeyTime(float32_t fireKeyTime);
	void SetStrength(float32_t strength);

private:
	float32_t time_ = 0.0f;
	float32_t fireKeyTime_ = 0.0f;
	float32_t strength_ = 0.0f;
};