#pragma once
#include "../Object.h"

class CsvDataComp : public IComp {
public:
	using IComp::IComp;

	~CsvDataComp() = default;

	void Init() override;

	void Load() override;

	const std::vector<std::vector<int32_t>>& GetCsvData() const;

	void Debug(const std::string& guiName);

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

	const std::vector<int32_t>& at(size_t index) const;

public:
	std::string fileName;
private:
	std::vector<std::vector<int32_t>> csvData_;

#ifdef _DEBUG
	std::vector<std::filesystem::path> filePaths_;
#endif // _DEBUG
};