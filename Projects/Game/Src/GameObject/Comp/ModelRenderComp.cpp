#include "ModelRenderComp.h"
#include "Drawers/DrawerManager.h"
#include "RenderDataComp.h"

#include "TransformComp.h"

void ModelRenderComp::Init() {
	transformComp_ = object_.AddComp<TransformComp>();
	renderDataComp_ = object_.AddComp<RenderDataComp>();
}

void ModelRenderComp::Draw() {
	const Camera& camera = *object_.GetCamera();

	model_->Draw(
		transformComp_->GetMatrix(),
		camera.GetViewProjection(),
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
