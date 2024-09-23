#pragma once
#include "../Object.h"
class LineComp : public IComp {
public:
	using IComp::IComp;

	~LineComp() = default;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;
	
	void Debug(const std::string& guiName);

	float32_t GetLenghth() const;
	Vector3 GetDirection() const;

public:
	Vector3 start;
	Vector3 end;
};
