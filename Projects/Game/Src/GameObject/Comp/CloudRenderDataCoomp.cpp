#include "CloudRenderDataCoomp.h"

void CloudRenderDataCoomp::Save(nlohmann::json& json) {
	SaveCompName(json);
	json["density"] = vec.x;
	json["discard"] = vec.y;
	json["min"] = vec.z;
}

void CloudRenderDataCoomp::Load(nlohmann::json& json) {
	vec.x = json["density"].get<float32_t>();
	vec.y = json["discard"].get<float32_t>();
	vec.z = json["min"].get<float32_t>();
}

void CloudRenderDataCoomp::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef USE_DEBUG_CODE
	if (ImGui::TreeNode(guiName.c_str())) {
		ImGui::DragFloat("density", &vec[0], 0.01f, 0.0f, 1000.0f);
		ImGui::DragFloat("discard", &vec[1], 0.001f, 0.0f, 1000.0f);
		ImGui::DragFloat("min", &vec[2], 0.001f, 0.0f, 1.0f);

		ImGui::TreePop();
	}
#endif // USE_DEBUG_CODE
}
