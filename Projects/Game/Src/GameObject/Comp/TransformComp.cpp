#include "TransformComp.h"
#include "../Manager/TransformCompUpdater.h"

TransformComp::TransformComp(Object* const object):
	IComp(object),
	scale(Vector3::kIdentity),
	rotate(),
	translate(),
	worldMatrix_(Mat4x4::kIdentity),
	parent_(nullptr),
	children_()
{
	TransformCompUpdater::GetInstance()->Set(this);
}

TransformComp::~TransformComp()
{
	TransformCompUpdater::GetInstance()->Erase(this);
}

void TransformComp::Init()
{
	
}

void TransformComp::UpdateMatrix()
{
	worldMatrix_ = Mat4x4::MakeAffin(scale, rotate, translate);

	if (parent_.have()) {
		worldMatrix_ *= parent_->worldMatrix_;
	}
}

void TransformComp::SetParent(Lamb::SafePtr<TransformComp>& parent)
{
	// もしparent_がnullじゃないかつ、今もっているやつと違うものかつ、親のchildrenに自身を持っているか
	if (parent_.have() and parent_ != parent and parent_->children_.contains(this)) {
		parent_->children_.erase(this);
	}
	parent_ = parent;
	if (parent_.have()) {
		parent_->children_.insert(this);
	}
}
