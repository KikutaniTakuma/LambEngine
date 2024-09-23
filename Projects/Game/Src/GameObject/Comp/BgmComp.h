#pragma once
#include "../Object.h"
class BgmComp : public IComp {
public:
	using IComp::IComp;

	~BgmComp() = default;

public:
	void Init() override;

	void FirstUpdate() override;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

public:
	Lamb::SafePtr<class AudioComp> audioComp_;
};
