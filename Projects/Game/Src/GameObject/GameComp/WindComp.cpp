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

		childrenComp_->AddObject(addObject.release());
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

			if (appearNum <= count) {
				break;
			}
		}
	}
}

Vector3 WindComp::GetDirection() const
{
	return (direction_ * obbComp_->GetObbComp().GetTransformComp().rotate).Normalize();
}

void WindComp::Save(nlohmann::json& json)
{
	SetCompName<WindComp>(json);
	json["positionRange"] = nlohmann::json::array();
	json["positionRange"].push_back(nlohmann::json::array());
	json["positionRange"].push_back(nlohmann::json::array());
	for (auto& i : positionRange_.first) {
		json["positionRange"][0].push_back(i);
	}
	for (auto& i : positionRange_.second) {
		json["positionRange"][1].push_back(i);
	}

	json["lengthRange"] = nlohmann::json::array();
	json["lengthRange"].push_back(lengthRange_.first);
	json["lengthRange"].push_back(lengthRange_.second);

	json["speedRange"] = nlohmann::json::array();
	json["speedRange"].push_back(speedRange_.first);
	json["speedRange"].push_back(speedRange_.second);

	json["appearNumberRange"] = nlohmann::json::array();
	json["appearNumberRange"].push_back(appearNumberRange_.first);
	json["appearNumberRange"].push_back(appearNumberRange_.second);

	json["appearNum"] = appearNum_;
}

void WindComp::Load(nlohmann::json& json)
{
	for (size_t i = 0; i < json["positionRange"][0].size(); i++) {
		positionRange_.first[i] = json["positionRange"][0][i].get<float32_t>();
	}
	for (size_t i = 0; i < json["positionRange"][1].size(); i++) {
		positionRange_.second[i] = json["positionRange"][1][i].get<float32_t>();
	}

	lengthRange_.first = json["lengthRange"][0].get<float32_t>();
	lengthRange_.second = json["lengthRange"][1].get<float32_t>();

	speedRange_.first = json["speedRange"][0].get<float32_t>();
	speedRange_.second = json["speedRange"][1].get<float32_t>();

	appearNumberRange_.first = static_cast<uint32_t>(json["appearNumberRange"][0].get<int32_t>());
	appearNumberRange_.second = static_cast<uint32_t>(json["appearNumberRange"][1].get<int32_t>());

	appearNum_ = static_cast<uint32_t>(json["appearNum"].get<int32_t>());
}
