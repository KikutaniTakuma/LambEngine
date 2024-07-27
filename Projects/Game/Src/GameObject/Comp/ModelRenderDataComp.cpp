#include "ModelRenderDataComp.h"



void ModelRenderDataComp::Init()
{
	type = BlendType::kNone;
	color = Vector4::kIdentity;
	isLighting = true;
}

void ModelRenderDataComp::Save(nlohmann::json& json)
{
	SetCompName<ModelRenderDataComp>(json);
	json["BlendType"] = kBlendTypeStrs[size_t(type)];
	json["color"] = nlohmann::json::array();
	for (auto& i : color) {
		json["color"].push_back(i);
	}
	json["isLighting"] = isLighting;
	json["fileName"] = fileName;
}

