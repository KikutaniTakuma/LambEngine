#include "QuadComp.h"
#include "TransformComp.h"
#include "Input/Mouse/Mouse.h"

void QuadComp::Init()
{
	transformComp_ = object_.AddComp<TransformComp>();
}

bool QuadComp::IsCollision(const Vector2& pos)
{
	Vector3 min = transformComp_->translate - transformComp_->scale;
	Vector3 max = transformComp_->translate + transformComp_->scale;

	return (min.x < pos.x and pos.x < max.x) and (min.y < pos.y and pos.y < max.y);
}

bool QuadComp::IsCollisionMouse(const Mat4x4& view, const Mat4x4& ndc) {
	Vector3 mousePos = Mouse::GetInstance()->GetPos();
	mousePos *= view * ndc;
	Vector3 min = transformComp_->translate - transformComp_->scale;
	Vector3 max = transformComp_->translate + transformComp_->scale;

	return (min.x < mousePos.x and mousePos.x < max.x) and (min.y < mousePos.y and mousePos.y < max.y);
}

void QuadComp::Save(nlohmann::json& json)
{
	SetCompName<QuadComp>(json);
}

void QuadComp::Load([[maybe_unused]]nlohmann::json& json)
{

}
