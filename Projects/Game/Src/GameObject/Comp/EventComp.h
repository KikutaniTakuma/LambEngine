#pragma once

#include "../Object.h"

class EventComp : public IComp {
public:
	using IComp::IComp;

	virtual ~EventComp() = default;

	virtual void Init() override;

	virtual void Event() override;

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

protected:
	Lamb::SafePtr<class ObbComp> obbComp;
	Lamb::Flg isEvent;
};