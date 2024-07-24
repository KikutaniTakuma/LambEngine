#include "CsvDataComp.h"
#include "Utils/FileUtils.h"

void CsvDataComp::Load(const std::string& fileName) {
	csvData_ = Lamb::LoadCsv(fileName);
}

const std::vector<std::vector<int32_t>>& CsvDataComp::GetCsvData() const {
	return csvData_;
}
