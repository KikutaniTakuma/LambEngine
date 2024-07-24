#include "Camera2DComp.h"

void Camera2DComp::Init()
{
	transform_ = object_.AddComp<decltype(transform_)::type>();
}

void Camera2DComp::LastUpdate()
{
	cameraMatrix_ = transform_->GetWorldMatrix().Inverse() * Mat4x4::MakeOrthographic(width_, height_, nearClip_, farClip_);
}
