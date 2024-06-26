#pragma once
#include "../Object.h"
#include "Drawers/Model/Model.h"


class ModelRenderComp : public IComp{
public:
	using IComp::IComp;

	~ModelRenderComp() = default;

	void Init() override;

	void Draw() override;

	void Load();

	void SetFileNmae(const std::string& fileName) {
		fileName_ = fileName;
	}

private:
	std::string fileName_;

	Lamb::SafePtr<Model> model_;

	Lamb::SafePtr<class RenderDataComp> renderDataComp_;

	Lamb::SafePtr<class TransformComp> transformComp_;
};