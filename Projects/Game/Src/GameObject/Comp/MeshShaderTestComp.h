#pragma once
#include "../Object.h"
#include "Drawer/Other/MeshShaderTest.h"

class MeshShaderTestComp : public IComp {
public:
	using IComp::IComp;

	~MeshShaderTestComp() = default;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;
	void Load() override;

public:
	void Init() override;

	void Draw(CameraComp* cameraComp) override;

	void Debug(const std::string& guiName) override;

private:
	Lamb::SafePtr<class ModelRenderDataComp> renderDataComp_;
	Lamb::SafePtr<class TransformComp> transform_;
	std::unique_ptr<MeshShaderTest> meshDrawer_;

#ifdef USE_DEBUG_CODE
	std::vector<std::filesystem::path> filePaths_;
#endif // USE_DEBUG_CODE
};
