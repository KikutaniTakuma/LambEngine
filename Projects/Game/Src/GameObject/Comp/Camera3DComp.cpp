#include "Camera3DComp.h"

void Camera3DComp::Init() {
	transform_ = object_.AddComp<decltype(transform_)::type>();
}

void Camera3DComp::LastUpdate()
{
	cameraMatrix_ = transform_->GetMatrix().Inverse() * Mat4x4::MakePerspectiveFov(fov_, aspectRatio_, nearClip_, farClip_);
}
