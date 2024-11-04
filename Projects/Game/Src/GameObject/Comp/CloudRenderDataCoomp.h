#pragma once
#include "../Object.h"
class CloudRenderDataCoomp : public IComp {
public:
	using IComp::IComp;

	~CloudRenderDataCoomp() = default;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

public:
	void Debug(const std::string& guiName);

public:
	Vector3 vec;
};
