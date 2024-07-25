#pragma once
#include "ItemComp.h"
class SailComp : public ItemComp {
public:
	enum class Direction {

	};

public:
	~SailComp() = default;

public:
	void Init() override;

	void Event() override;

	void SetOpenKeyTime(float32_t openKeyTime);
	void SetStrength(float32_t strength);

private:
	Lamb::Flg isOpen_;
	float32_t time_ = 0.0f;
	float32_t openKeyTime_ = 0.0f;
	float32_t strength_ = 0.0f;

	Lamb::SafePtr<class ObbComp> obbComp_;
};