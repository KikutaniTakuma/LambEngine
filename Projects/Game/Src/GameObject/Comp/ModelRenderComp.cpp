#include "ModelRenderComp.h"
#include "Drawers/DrawerManager.h"
#include "ModelRenderDataComp.h"

#include "TransformComp.h"
#include "CameraComp.h"

void ModelRenderComp::Init() {
	transformComp_ = object_.AddComp<TransformComp>();
	renderDataComp_ = object_.AddComp<ModelRenderDataComp>();
}

void ModelRenderComp::Draw(CameraComp* cameraComp) {
	model_->Draw(
		transformComp_->GetWorldMatrix(),
		cameraComp->GetCameraMatrix(),
		renderDataComp_->color.GetColorRGBA(),
		renderDataComp_->type,
		renderDataComp_->isLighting
	);
}

void ModelRenderComp::Load() {
	if (fileName_.empty()) {
		throw Lamb::Error::Code<ModelRenderComp>("does not set fileName", ErrorPlace);
	}
	Lamb::SafePtr drawerManager = DrawerManager::GetInstance();
	drawerManager->LoadModel(fileName_);
	model_ = drawerManager->GetModel(fileName_);
}
