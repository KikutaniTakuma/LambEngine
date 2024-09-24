#pragma once
#include "../Object.h"
#include "Drawers/Other/MeshShaderTest.h"

class MeshShaderTestComp : public IComp {
public:
	using IComp::IComp;

	~MeshShaderTestComp() = default;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

public:
	void Init() override;

	void Draw(CameraComp* cameraComp) override;

private:
	Lamb::SafePtr<class TransformComp> transform_;
	std::unique_ptr<MeshShaderTest> meshDrawer;
};
