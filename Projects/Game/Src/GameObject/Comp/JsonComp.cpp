#include "JsonComp.h"
#include "Utils/FileUtils.h"

#ifdef USE_DEBUG_CODE
#include "imgui.h"
#endif // USE_DEBUG_CODE


void JsonComp::Init() {
#ifdef USE_DEBUG_CODE
	filePaths_ = Lamb::GetFilePathFormDir("./", ".json");
#endif // USE_DEBUG_CODE
	Load();
}

void JsonComp::Load()
{
	if (fileName.empty()) {
		return;
	}
	jsonData_ = Lamb::LoadJson(fileName);
}

nlohmann::json& JsonComp::GetJsonData() {
	return jsonData_;
}

const nlohmann::json& JsonComp::GetJsonData() const {
	return jsonData_;
}

void JsonComp::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef USE_DEBUG_CODE
	if (ImGui::TreeNode(guiName.c_str())) {
		if (ImGui::Button("再読み込み")) {
			filePaths_ = Lamb::GetFilePathFormDir("./", ".json");
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

#endif // USE_DEBUG_CODE
}

void JsonComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
	json["fileName"] = fileName;
}

void JsonComp::Load(nlohmann::json& json)
{
	fileName = json["fileName"].get<std::string>();
}
