#include "UIDrawComp.h"
#include "Utils/EngineInfo.h"
#include "Drawer/DrawerManager.h"
#include "../Comp/SpriteRenderDataComp.h"
#include "../Comp/TransformComp.h"

void UIDrawComp::Init()
{
	tex2D_ = DrawerManager::GetInstance()->GetTexture2D();
	renderDataComp_ = object_.AddComp<SpriteRenderDataComp>();
	transformComp_ = object_.AddComp<TransformComp>();
}

void UIDrawComp::Event()
{
}

void UIDrawComp::Draw([[maybe_unused]]CameraComp* cameraComp)
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

void UIDrawComp::Save([[maybe_unused]] nlohmann::json& json)
{
	SaveCompName(json);
}

void UIDrawComp::Load([[maybe_unused]] nlohmann::json& json)
{
}
