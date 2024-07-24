#pragma once
#include "../Object.h"
#include "json.hpp"

class JsonCmop : public IComp {
public:
	using IComp::IComp;

	~JsonCmop() = default;

	void Load(const std::string& fileName);

	nlohmann::json& GetJsonData();
	const nlohmann::json& GetJsonData() const;

private:
	nlohmann::json jsonData_;
};