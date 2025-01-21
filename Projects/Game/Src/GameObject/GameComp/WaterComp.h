#pragma once

#include "../Object.h"
class WaterComp : public IComp {
public:
	using IComp::IComp;

	~WaterComp() = default;

public:
	void Init() override;

	void Update() override;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

private:
	Lamb::SafePtr<class TransformComp> transformComp_;
};
