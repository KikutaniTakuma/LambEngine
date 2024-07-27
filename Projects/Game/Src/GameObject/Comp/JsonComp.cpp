#include "JsonComp.h"
#include "Utils/FileUtils.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void JsonCmop::Init() {
#ifdef _DEBUG
	filePaths_ = Lamb::GetFilePathFormDir("./", ".json");
#endif // _DEBUG
	Load();
}

void JsonCmop::Load()
{
	if (fileName.empty()) {
		return;
	}
	jsonData_ = Lamb::LoadJson(fileName);
}

nlohmann::json& JsonCmop::GetJsonData() {
	return jsonData_;
}

const nlohmann::json& JsonCmop::GetJsonData() const {
	return jsonData_;
}

void JsonCmop::Debug([[maybe_unused]]const std::string& guiName) {
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

void JsonCmop::Save(nlohmann::json& json)
{
	SetCompName<JsonCmop>(json);
	json["fileName"] = fileName;
}
