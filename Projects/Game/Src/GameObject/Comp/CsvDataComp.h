#pragma once
#include "../Object.h"

class CsvDataComp : public IComp {
public:
	using IComp::IComp;

	~CsvDataComp() = default;

	void Load(const std::string& fileName);

	const std::vector<std::vector<int32_t>>& GetCsvData() const;

private:
	std::vector<std::vector<int32_t>> csvData_;
};