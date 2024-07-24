#pragma once
#include "../Object.h"

class CsvDataComp : public IComp {
public:
	using IComp::IComp;

	~CsvDataComp() = default;

#ifdef _DEBUG
	void Init() override;
#endif // _DEBUG

	void Load(const std::string& fileName);

	const std::vector<std::vector<int32_t>>& GetCsvData() const;

	void Debug(const std::string& guiName);

private:
	std::vector<std::vector<int32_t>> csvData_;

#ifdef _DEBUG
	std::vector<std::filesystem::path> filePaths_;
#endif // _DEBUG
};