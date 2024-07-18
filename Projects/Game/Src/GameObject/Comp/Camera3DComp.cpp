#include "Camera3DComp.h"
#include "imgui.h"

void Camera3DComp::Init() {
	transform_ = object_.AddComp<decltype(transform_)::type>();
}

void Camera3DComp::LastUpdate()
{
	cameraMatrix_ = transform_->GetMatrix().Inverse() * Mat4x4::MakePerspectiveFov(fov_, aspectRatio_, nearClip_, farClip_);
}

const Vector3& Camera3DComp::GetPos() const
{
	return transform_->translate;
}

void Camera3DComp::Debug([[maybe_unused]]const std::string& guiName){
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("scale", transform_->scale.data(), 0.01f);
	ImGui::DragFloat4("rotate", transform_->rotate.data(), 0.01f);
	ImGui::DragFloat3("translate", transform_->translate.data(), 0.01f);
	//transform_->rotate = Quaternion::EulerToQuaternion(rotate_);
	ImGui::End();
#endif // _DEBUG
}
