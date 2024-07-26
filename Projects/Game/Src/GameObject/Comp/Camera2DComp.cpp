#include "Camera2DComp.h"
#include "CameraComp.h"

void Camera2DComp::Init() {
	cameraComp_ = object_.AddComp<CameraComp>();
}

void Camera2DComp::Update() {
	othographicMatrix_ = Mat4x4::MakeOrthographic(width_, height_, nearClip_, farClip_);
	cameraComp_->SetNdcMatix(othographicMatrix_);
}

const Mat4x4& Camera2DComp::GetToNdcMatrix() const
{
	return othographicMatrix_;
}
