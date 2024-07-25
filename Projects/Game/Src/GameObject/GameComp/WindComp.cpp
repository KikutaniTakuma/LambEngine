#include "WindComp.h"
#include "../Comp/ObbPushComp.h"
#include "../Comp/ChildrenObjectComp.h"
#include "../Comp/TransformComp.h"
#include "WindNodeComp.h"

#include "Utils/Random.h"

void WindComp::Init() {
	childrenComp_ = object_.AddComp<decltype(childrenComp_)::type>();
	obbComp_ = object_.AddComp<decltype(obbComp_)::type>();
	transformComp_ = object_.AddComp<decltype(transformComp_)::type>();

	windNodeComp_.reserve(kMaxWindowNodeSize_);

	for (size_t i = 0; i < kMaxWindowNodeSize_; i++) {
		std::unique_ptr<Object> addObject = std::make_unique<Object>();
		Lamb::SafePtr windNode = addObject->AddComp<WindNodeComp>();

		windNodeComp_.insert(windNode);

		childrenComp_->AddObject(std::move(addObject));
	}

	lengthRange_.first = 1.0f;
	lengthRange_.second = transformComp_->scale.Length() * 0.5f;

	speedRange_.first = 3.0f;
	speedRange_.second = 6.0f;

	appearNumberRange_.first = 2;
	appearNumberRange_.second = 3;

	appearFrequencyRange_.first = 0;
	appearFrequencyRange_.second = 99;
	appearNum_ = 2;

	positionRange_.first = transformComp_->translate - transformComp_->scale;
	positionRange_.second = transformComp_->translate + transformComp_->scale;

	if (direction_.z == 1.0f) {
		positionRange_.second.z = positionRange_.first.z;
	}
	else if (direction_.z == -1.0f) {
		positionRange_.first.z = positionRange_.second.z;
	}
	else if (direction_.x == 1.0f) {
		positionRange_.second.x = positionRange_.first.x;
	}
	else if (direction_.x == -1.0f) {
		positionRange_.first.x = positionRange_.second.x;
	}
}

void WindComp::FirstUpdate() {
	// 生成するか
	if (Lamb::Random(appearFrequencyRange_.first, appearFrequencyRange_.second) < appearNum_) {
		//生成する数
		uint32_t appearNum = Lamb::Random(appearNumberRange_.first, appearNumberRange_.second);
		for (uint32_t count = 0; auto & i : windNodeComp_) {
			if (not i->GetIsActive()) {
				i->direction = direction_;
				i->minRange = transformComp_->translate - transformComp_->scale;
				i->maxRange = transformComp_->translate + transformComp_->scale;
				i->start = Lamb::Random(positionRange_.first, positionRange_.second);
				i->length = Lamb::Random(lengthRange_.first, lengthRange_.second);
				i->speed = Lamb::Random(speedRange_.first, speedRange_.second);

				i->Start();

				count++;
			}
		}
	}
}

Vector3 WindComp::GetDirection() const
{
	return (direction_ * obbComp_->GetObbComp().GetTransformComp().rotate).Normalize();
}
