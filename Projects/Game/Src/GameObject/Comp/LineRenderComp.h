#pragma once

#include "../Object.h"
class LineRenderComp : public IComp {
public:
	using IComp::IComp;

	~LineRenderComp() = default;

	void Init() override;

	void Draw(CameraComp* camera);

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

private:
	Lamb::SafePtr<class LineRenderDataComp> lineRenderDataComp_;
	Lamb::SafePtr<class LineComp> lineComp_;
};
