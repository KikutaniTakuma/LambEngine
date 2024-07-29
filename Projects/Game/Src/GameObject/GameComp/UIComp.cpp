#include "UIComp.h"
#include "Utils/EngineInfo.h"
#include "Drawers/DrawerManager.h"
#include "../Comp/SpriteRenderDataComp.h"
#include "../Comp/TransformComp.h"

void UIComp::Init()
{
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();
	renderDataComp_ = object_.AddComp<SpriteRenderDataComp>();
	transformComp_ = object_.AddComp<TransformComp>();
}

void UIComp::Event()
{
}

void UIComp::Draw([[maybe_unused]]CameraComp* cameraComp)
{
	Vector2 clientSize = Lamb::ClientSize();
	Mat4x4 cameraMatrix = Mat4x4::MakeTranslate(Vector3::kZIdentity * 10.0f) * Mat4x4::MakeOrthographic(clientSize.x, clientSize.y, 1.0f, 100.0f);

	tex2D_->Draw(
		renderDataComp_->GetOffsetMatrix() * transformComp_->GetWorldMatrix(),
		renderDataComp_->uvTransform.GetMatrix(),
		cameraMatrix,
		renderDataComp_->texHandle,
		renderDataComp_->color.GetColorRGBA(),
		renderDataComp_->type
	);
}

void UIComp::Save([[maybe_unused]] nlohmann::json& json)
{
}

void UIComp::Load([[maybe_unused]] nlohmann::json& json)
{
}
