#include "ObbPushComp.h"

void ObbPushComp::Init()
{
	obbComp_ = object_.AddComp<ObbComp>();
}

void ObbPushComp::Collision(Lamb::SafePtr<ObbComp> other) {
	if (not pushTag_.empty() and other->getObject().HasTag(pushTag_)) {
		Vector3 pushvector;
		obbComp_->IsCollision(other.get(), pushvector);
		other->GetTransformComp().translate += pushvector;
	}
	else {
		Vector3 pushvector;
		other->IsCollision(obbComp_.get(), pushvector);
		obbComp_->GetTransformComp().translate += pushvector;
	}
}

void ObbPushComp::SetPushTag(const std::string& pushTag) {
	pushTag_ = pushTag;
}
