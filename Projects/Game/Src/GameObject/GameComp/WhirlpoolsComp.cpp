#include "WhirlpoolsComp.h"
#include "../Comp/ObbPushComp.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void WhirlpoolsComp::Init() {
	obbComp_ = object_.AddComp<ObbPushComp>();
	spriteRenderComp_ = object_.AddComp<SpriteRenderComp>();
}

void WhirlpoolsComp::Update() {
	rotate_ += rotateSpeed_;
	transformComp_->rotate *= Quaternion::MakeRotateYAxis(rotate_);
}

bool WhirlpoolsComp::CanPathThrough(float32_t speed)
{
	return pathThroughSpeed_ <= speed;
}

void WhirlpoolsComp::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	if (ImGui::TreeNode(guiName.c_str())) {
		rotateSpeed_ *= Lamb::Math::toDegree<float32_t>;
		ImGui::DragFloat("rotateSpeed(Degree)", &rotateSpeed_, 0.1f, -360.0f, 360.0f);
		rotateSpeed_ *= Lamb::Math::toRadian<float32_t>;

		ImGui::TreePop();
	}
#endif // _DEBUG

}

void WhirlpoolsComp::Save(nlohmann::json& json)
{
	SetCompName<WhirlpoolsComp>(json);
	json["pathThroughSpeed"] = pathThroughSpeed_;
	json["rotateSpeed"] = rotateSpeed_;
}

void WhirlpoolsComp::Load(nlohmann::json& json)
{
	pathThroughSpeed_ = json["pathThroughSpeed"].get<float32_t>();
	rotateSpeed_ = json["rotateSpeed"].get<float32_t>();
}
