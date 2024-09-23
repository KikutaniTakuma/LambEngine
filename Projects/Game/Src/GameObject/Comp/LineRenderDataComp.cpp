#include "LineRenderDataComp.h"
#include "LineComp.h"

void LineRenderDataComp::Init()
{
	lineComp_ = object_.AddComp<LineComp>();
}

void LineRenderDataComp::Save(nlohmann::json& json) {
	SaveCompName(json);
	json["color"] = nlohmann::json::array();
	for (auto& i : color) {
		json["color"].push_back(i);
	}
	json["isDepth"] = isDepth;
}

void LineRenderDataComp::Load(nlohmann::json& json) {
	for (size_t i = 0; i < color.size(); i++) {
		color[i] = json["color"][i].get<float32_t>();
	}
	isDepth = json["isDepth"].get<bool>();
}

void LineRenderDataComp::Debug([[maybe_unused]]const std::string& guiName)
{
#ifdef _DEBUG
	if(ImGui::TreeNode(guiName.c_str())){
		ImGui::DragFloat3("start", lineComp_->start.data(), 0.01f);
		ImGui::DragFloat3("end", lineComp_->end.data(), 0.01f);
		ImGui::ColorEdit4("color", color.data());
		ImGui::Checkbox("isDepth", &isDepth);
		ImGui::TreePop();
	}
#endif // _DEBUG
}
