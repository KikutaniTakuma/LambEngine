#include "MeshShaderTestComp.h"
#include "TransformComp.h"
#include "CameraComp.h"

void MeshShaderTestComp::Save(nlohmann::json& json) {
	SaveCompName(json);
}

void MeshShaderTestComp::Load([[maybe_unused]]nlohmann::json& json)
{
}

void MeshShaderTestComp::Init() {
	meshDrawer = std::make_unique<MeshShaderTest>();
	meshDrawer->Load();

	transform_ = object_.AddComp<TransformComp>();
}

void MeshShaderTestComp::Draw(CameraComp* cameraComp)
{
	meshDrawer->Draw(
		transform_->GetWorldMatrix(),
		cameraComp->GetCameraMatrix(),
		BlendType::kNone
	);
}
