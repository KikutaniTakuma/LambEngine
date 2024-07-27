#include "ObbPushComp.h"

void ObbPushComp::Init()
{
	obbComp_ = object_.AddComp<ObbComp>();
}

void ObbPushComp::Collision(Lamb::SafePtr<ObbComp> other) {
	bool isPush = false;

	for (const auto& i : pushTags_) {
		if (other->getObject().HasTag(i)) {
			isPush = true;
			break;
		}
	}

	if (isPush) {
		Vector3 pushvector;
		if (obbComp_->IsCollision(other.get(), pushvector)) {
			other->GetTransformComp().translate += pushvector;
		}
	}
	else {
		other->IsCollision(obbComp_.get());
	}
}

void ObbPushComp::SetPushTag(const std::string& pushTag) {
	if (not pushTags_.contains(pushTag)) {
		pushTags_.insert(pushTag);
	}
}

void ObbPushComp::ErasePushTag(const std::string& pushTag) {
	if (pushTags_.contains(pushTag)) {
		pushTags_.erase(pushTag);
	}
}

void ObbPushComp::Save(nlohmann::json& json)
{
	SetCompName<ObbPushComp>(json);
	json["pushTags"] = nlohmann::json::array();
	for (auto& i : pushTags_) {
		json["pushTags"].push_back(i);
	}
}
