#include "SpriteRenderComp.h"
#include "Drawers/DrawerManager.h"
#include "SpriteRenderDataComp.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "TransformComp.h"

void SpriteRenderComp::Init() {
	transformComp_ = object_.AddComp<TransformComp>();
	renderDataComp_ = object_.AddComp<SpriteRenderDataComp>();

	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();
}

void SpriteRenderComp::Draw() {
	tex2D_->Draw(
		transformComp_->GetWorldMatrix(),
		renderDataComp_->uvTransform.GetMatrix(),
		object_.GetCameraMatrix(),
		renderDataComp_->texHandle,
		renderDataComp_->color.GetColorRGBA(),
		renderDataComp_->type
	);
}

void SpriteRenderComp::Load(const std::string& fileName) {
	renderDataComp_->fileName = fileName;
	renderDataComp_->texHandle = TextureManager::GetInstance()->GetHandle(renderDataComp_->fileName);
}

