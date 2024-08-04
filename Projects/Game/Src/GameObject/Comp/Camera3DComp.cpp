#include "Camera3DComp.h"
#include "CameraComp.h"


void Camera3DComp::Init() {
	cameraComp_ = object_.AddComp<CameraComp>();

	fov_ = 0.45f;
	aspectRatio_ = 720.0f / 1280.0f;
	nearClip_ = 0.1f;
	farClip_ = 1000.0f;

	projectionMatrix_ = Mat4x4::MakePerspectiveFov(fov_, aspectRatio_, nearClip_, farClip_);
}

void Camera3DComp::Update() {
	projectionMatrix_ = Mat4x4::MakePerspectiveFov(fov_, aspectRatio_, nearClip_, farClip_);
	cameraComp_->SetNdcMatix(projectionMatrix_);
}

void Camera3DComp::Save(nlohmann::json& json) {
	SaveCompName(json);
	json["fov"] = fov_;
	json["aspectRatio"] = aspectRatio_;
	json["nearClip"] = nearClip_;
	json["farClip"] = farClip_;
}

void Camera3DComp::Load(nlohmann::json& json) {
	fov_ = json["fov"].get<float32_t>();
	aspectRatio_ = json["aspectRatio"].get<float32_t>();
	nearClip_ = json["nearClip"].get<float32_t>();
	farClip_ = json["farClip"].get<float32_t>();
}

const Mat4x4& Camera3DComp::GetToNdcMatrix() const
{
	return projectionMatrix_;
}
