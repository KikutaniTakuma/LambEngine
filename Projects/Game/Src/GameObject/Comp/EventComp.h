#pragma once

#include "../Object.h"

class EventComp : public IComp {
public:
	using IComp::IComp;

	virtual ~EventComp() = default;

	virtual void Init() override;

	virtual void Event() override;

protected:
	Lamb::SafePtr<class ObbComp> obbComp;
	Lamb::Flg isEvent;
};