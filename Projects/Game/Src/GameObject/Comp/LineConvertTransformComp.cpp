#include "LineConvertTransformComp.h"

#include "TransformComp.h"
#include "LineComp.h"

void LineConvertTransformComp::Init() {
	transformComp_ = object_.AddComp<TransformComp>();
	lineComp_ = object_.AddComp<LineComp>();
}

void LineConvertTransformComp::LastUpdate() {
	transformComp_->translate = Vector3::Lerp(lineComp_->start, lineComp_->end, 0.5f);
	transformComp_->scale = { (lineComp_->start - lineComp_->end).Length(), transformComp_->scale.y, transformComp_->scale.z };

	Vector3 to = (lineComp_->end - lineComp_->start).Normalize();
#ifdef _DEBUG
	transformComp_->eulerRotate = Quaternion::DirectionToDirection(Vector3::kXIdentity, to).ToEuler();
#else
	transformComp_->rotate = Quaternion::DirectionToDirection(Vector3::kXIdentity, to);
#endif // _DEBUG
}

void LineConvertTransformComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
}

void LineConvertTransformComp::Load([[maybe_unused]]nlohmann::json& json){}
