#include "BaseDrawer.h"

#include "Engine/Graphics/RenderContextManager/RenderContextManager.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


BaseDrawer::BaseDrawer() :
	scale{ Vector3::kIdentity },
	rotate{},
	translate{},
	color(std::numeric_limits<uint32_t>::max()),
	renderSet(nullptr)
{}

void BaseDrawer::Draw(const Mat4x4& camera, BlendType blend)
{
	RenderData* render = renderSet->GetRenderContext(blend);

	render->SetWVPMatrix({
		Mat4x4::MakeAffin(scale, rotate, translate),
		camera
		}
	);

	render->SetColor(color);

	render->AddDrawCount();
}

void BaseDrawer::Debug([[maybe_unused]]const std::string& guiName)
{
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("スケール", &scale.x,0.01f);
	ImGui::DragFloat3("回転", &rotate.x,0.01f);
	ImGui::DragFloat3("ポジション", &translate.x,0.01f);
	ImGui::End();
#endif // _DEBUG
}
