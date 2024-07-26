#include "Camera2DComp.h"

void Camera2DComp::Init() {
	CameraComp::Init();
}

void Camera2DComp::LastUpdate()
{
	viewMatrix_ = transform_->GetWorldMatrix().Inverse();
	othographicMatrix_ = Mat4x4::MakeOrthographic(width_, height_, nearClip_, farClip_);
	cameraMatrix_ = viewMatrix_ * othographicMatrix_;
}

const Mat4x4& Camera2DComp::GetToNdcMatrix() const
{
	return othographicMatrix_;
}
