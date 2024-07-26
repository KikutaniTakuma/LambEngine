#include "CameraComp.h"

void CameraComp::Init() {
	transform_ = object_.AddComp<decltype(transform_)::type>();
	viewMatrix_ = transform_->GetWorldMatrix().Inverse();
}

const Mat4x4& CameraComp::GetViewMatrix() const
{
	return viewMatrix_;
}

TransformComp& CameraComp::GetTransformComp() {
	return *transform_;
}

const TransformComp& CameraComp::GetTransformComp() const {
	return *transform_;
}
