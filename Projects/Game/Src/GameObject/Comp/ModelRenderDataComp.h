#pragma once
#include "../Object.h"
#include "Drawers/Model/Model.h"


class ModelRenderDataComp : public IComp {
public:
	using IComp::IComp;

	~ModelRenderDataComp() = default;

	void Init() override;

	void Debug(const std::string& guiName) override;

public:
	BlendType type;
	Vector4 color;
	bool isLighting;
};