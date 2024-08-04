#include "ModelRenderDataComp.h"



void ModelRenderDataComp::Init()
{
	type = BlendType::kNone;
	color = Vector4::kIdentity;
	isLighting = true;
}

void ModelRenderDataComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
	json["BlendType"] = kBlendTypeStrs[size_t(type)];
	json["color"] = nlohmann::json::array();
	for (auto& i : color) {
		json["color"].push_back(i);
	}
	json["isLighting"] = isLighting;
	json["fileName"] = fileName;
}

void ModelRenderDataComp::Load(nlohmann::json& json)
{
	std::string&& blendType = json["BlendType"].get<std::string>();
	for (size_t count = 0; const auto& i : kBlendTypeStrs) {
		if (blendType == i) {
			type = BlendType(count);
			break;
		}
		count++;
	}

	for (size_t i = 0; i < json["color"].size(); i++) {
		color[i] = json["color"][0];
	}
	isLighting = json["isLighting"].get<bool>();
	fileName = json["fileName"].get<std::string>();
}

