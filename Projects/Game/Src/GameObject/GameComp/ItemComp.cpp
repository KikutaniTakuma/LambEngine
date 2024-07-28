#include "ItemComp.h"

#include "../Comp/TransformComp.h"

void ItemComp::Init() {
	transform_ = object_.AddComp<TransformComp>();
}

void ItemComp::Start() {
	isStart_ = true;
}

Vector3 ItemComp::GetVelocity() const
{
	return (Vector3::kXIdentity * transform_->rotate) * velocity_;
}

void ItemComp::Save(nlohmann::json& json)
{
	SetCompName<ItemComp>(json);
}
