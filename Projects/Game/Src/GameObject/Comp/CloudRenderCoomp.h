#pragma once
#include "../Object.h"
#include "Drawer/Other/CloudTex2D/CloudTex2D.h"

class CloudRenderCoomp : public IComp {
public:
	using IComp::IComp;

	~CloudRenderCoomp() = default;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

public:
	void Init() override;

	void Draw(CameraComp* cameraComp) override;

private:
	Lamb::SafePtr<class CloudRenderDataCoomp> cloudRenderDataCoomp_;
	Lamb::SafePtr<class SpriteRenderDataComp> spriteRenderDataComp_;
	Lamb::SafePtr<class TransformComp>		  transformComp_;

	std::unique_ptr<CloudTex2D> cloudTex2D_;
};
