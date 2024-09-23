#pragma once
#include "../Object.h"
class LineConvertTransformComp : public IComp {
public:
	using IComp::IComp;

	~LineConvertTransformComp() = default;

public:
	void Init() override;

	void LastUpdate() override;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

private:
	Lamb::SafePtr<class TransformComp> transformComp_;
	Lamb::SafePtr<class LineComp> lineComp_;

};
