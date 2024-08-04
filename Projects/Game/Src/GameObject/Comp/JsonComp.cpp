#include "JsonComp.h"
#include "Utils/FileUtils.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void JsonComp::Init() {
#ifdef _DEBUG
	filePaths_ = Lamb::GetFilePathFormDir("./", ".json");
#endif // _DEBUG
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
#ifdef _DEBUG
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

#endif // _DEBUG
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
