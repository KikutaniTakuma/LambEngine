#include "TransformComp.h"

void TransformComp::Init()
{
	
}

void TransformComp::Update()
{
	worldMatrix_ = Mat4x4::MakeAffin(scale, rotate, translate);

	if (parent_.have()) {
		worldMatrix_ *= parent_->worldMatrix_;
	}
}

void TransformComp::SetParent(Lamb::SafePtr<TransformComp>& parent)
{
	// もしparent_がnullじゃないかつ、今もっているやつと違うものかつ、親のchildに自身を持っているか
	if (parent_.have() and parent_ != parent and parent_->childlen_.contains(this)) {
		parent_->childlen_.erase(this);
	}
	parent_ = parent;
	if (parent_.have()) {
		parent_->childlen_.insert(this);
	}
}
