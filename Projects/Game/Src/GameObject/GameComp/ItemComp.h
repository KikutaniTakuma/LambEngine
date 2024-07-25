#pragma once
#include "../Object.h"
class ItemComp : public IComp {
public:
	using IComp::IComp;

	virtual ~ItemComp() = default;

public:
	void Start();

	float32_t GetVelocity() const;

protected:
	float32_t velocity_ = 0;

	Lamb::Flg isStart;
};
