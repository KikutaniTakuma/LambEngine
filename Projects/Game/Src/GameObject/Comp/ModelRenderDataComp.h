#pragma once
#include "../Object.h"
#include "Drawers/Model/Model.h"


class ModelRenderDataComp : public IComp {
public:
	using IComp::IComp;

	~ModelRenderDataComp() = default;

	void Init() override;

public:
	BlendType type;
	Vector4 color;
	bool isLighting;
	std::string fileName;
};