#pragma once
#include "../Object.h"
#include "Drawers/Model/Model.h"


class RenderDataComp : public IComp {
public:
	using IComp::IComp;

	~RenderDataComp() = default;

	void Init() override;

public:
	BlendType type;
	Vector4 color;
	bool isLighting;
};