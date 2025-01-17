#pragma once
#include "../Object.h"
#include "Drawer/Model/Model.h"


class ModelRenderComp : public IComp{
public:
	using IComp::IComp;

	~ModelRenderComp() = default;

	void Init() override;

	void Draw(CameraComp* cameraComp) override;

	void Load() override;

	void Debug(const std::string& guiName) override;

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

private:
	Lamb::SafePtr<Model> model_;

	Lamb::SafePtr<class ModelRenderDataComp> renderDataComp_;

	Lamb::SafePtr<class TransformComp> transformComp_;

#ifdef USE_DEBUG_CODE
	std::vector<std::filesystem::path> filePaths_;
#endif // USE_DEBUG_CODE
};