#pragma once
#include "../Object.h"
#include "Drawer/Model/Model.h"


class ModelRenderDataComp : public IComp {
public:
	using IComp::IComp;

	~ModelRenderDataComp() = default;

	void Init() override;

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

public:
	BlendType type;
	Vector4 color;
	bool isLighting = true;
	std::string fileName;
	bool isDraw = true;
};