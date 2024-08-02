#pragma once
#include "../Object.h"
#include "json.hpp"

class JsonComp : public IComp {
public:
	using IComp::IComp;

	~JsonComp() = default;

	void Init() override;


	void Load() override;

	nlohmann::json& GetJsonData();
	const nlohmann::json& GetJsonData() const;

	void Debug(const std::string& guiName);

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

public:
	std::string fileName;

private:
	nlohmann::json jsonData_;

#ifdef _DEBUG
	std::vector<std::filesystem::path> filePaths_;
#endif // _DEBUG

};