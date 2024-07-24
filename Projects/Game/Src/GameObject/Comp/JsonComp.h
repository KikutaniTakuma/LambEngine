#pragma once
#include "../Object.h"
#include "json.hpp"

class JsonCmop : public IComp {
public:
	using IComp::IComp;

	~JsonCmop() = default;

#ifdef _DEBUG
	void Init() override;
#endif // _DEBUG


	void Load(const std::string& fileName);

	nlohmann::json& GetJsonData();
	const nlohmann::json& GetJsonData() const;

	void Debug(const std::string& guiName);


private:
	nlohmann::json jsonData_;

#ifdef _DEBUG
	std::vector<std::filesystem::path> filePaths_;
#endif // _DEBUG

};