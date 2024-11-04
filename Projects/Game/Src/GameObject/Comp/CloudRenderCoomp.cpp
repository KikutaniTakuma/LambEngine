#include "CloudRenderCoomp.h"
#include "CloudRenderDataCoomp.h"
#include "SpriteRenderDataComp.h"
#include "TransformComp.h"
#include "CameraComp.h"


void CloudRenderCoomp::Save(nlohmann::json& json) {
	SaveCompName(json);
}

void CloudRenderCoomp::Load([[maybe_unused]]nlohmann::json& json)
{
}

void CloudRenderCoomp::Init() {
	cloudRenderDataCoomp_ = object_.AddComp<CloudRenderDataCoomp>();
	spriteRenderDataComp_ = object_.AddComp<SpriteRenderDataComp>();
	transformComp_ = object_.AddComp<TransformComp>();

	cloudTex2D_ = std::make_unique<CloudTex2D>();
	cloudTex2D_->Load();
}

void CloudRenderCoomp::Draw(CameraComp* cameraComp) {
	cloudTex2D_->Draw(
		transformComp_->GetWorldMatrix(),
		Mat4x4::kIdentity,
		cameraComp->GetCameraMatrix(),
		spriteRenderDataComp_->texHandle,
		cloudRenderDataCoomp_->vec,
		spriteRenderDataComp_->color.GetColorRGBA(),
		spriteRenderDataComp_->type
	);
}
