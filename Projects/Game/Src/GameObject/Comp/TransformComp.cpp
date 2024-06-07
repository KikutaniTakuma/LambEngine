#include "TransformComp.h"

void TransformComp::Init()
{
	
}

void TransformComp::LastUpdate()
{
	worldMatrix_ = Mat4x4::MakeAffin(scale, rotate, translate);

	if (parent_) {
		worldMatrix_ *= parent_->worldMatrix_;
	}
}

void TransformComp::SetParent(TransformComp* parent)
{
	parent_ = parent;
}
