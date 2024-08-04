#include "CsvDataComp.h"
#include "Utils/FileUtils.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

void CsvDataComp::Init()
{
#ifdef _DEBUG
	filePaths_ = Lamb::GetFilePathFormDir("./", ".csv");
#endif // _DEBUG
}

void CsvDataComp::Load() {
	if (fileName.empty()) {
		return;
	}
	csvData_ = Lamb::LoadCsv(fileName);
}

const std::vector<std::vector<int32_t>>& CsvDataComp::GetCsvData() const {
	return csvData_;
}

void CsvDataComp::Debug([[maybe_unused]] const std::string& guiName) {
#ifdef _DEBUG
	if (ImGui::TreeNode(guiName.c_str())) {
		if (ImGui::TreeNode("Data")) {
			for (const auto& line : csvData_) {
				for (auto element = line.begin(); element != line.end(); element++) {
					ImGui::Text("%d, ", *element);
					if (element != line.rbegin().base()) {
						ImGui::SameLine();
					}
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::Button("再読み込み")) {
			filePaths_ = Lamb::GetFilePathFormDir("./", ".csv");
		}

		for (auto& i : filePaths_) {
			bool isLoad = ImGui::Button("Load");
			ImGui::SameLine();
			ImGui::Text("%s", i.string().c_str());

			if (isLoad) {
				fileName = i.string().c_str();
				Load();
			}
		}

		ImGui::TreePop();
	}

#endif // _DEBUG
}

void CsvDataComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
	json["fileName"] = fileName;
}

void CsvDataComp::Load(nlohmann::json& json)
{
	fileName = json["fileName"].get<std::string>();
}

const std::vector<int32_t>& CsvDataComp::at(size_t index) const
{
	return csvData_[index];
}
