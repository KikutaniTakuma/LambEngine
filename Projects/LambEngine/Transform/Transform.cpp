#include "Transform.h"

#ifdef USE_IMGUI
#include "imgui.h"
#endif // USE_IMGUI


QuaternionTransform& QuaternionTransform::operator=(const Transform& transform) {
	this->scale = transform.scale;
	this->rotate = Quaternion::EulerToQuaternion(transform.rotate);
	this->translate = transform.translate;

	return *this;
}

Transform& Transform::operator=(const QuaternionTransform& transform) {
	this->scale = transform.scale;
	this->rotate = Quaternion::QuaternionToEuler(transform.rotate);
	this->translate = transform.translate;

	return *this;
}

Mat4x4 Transform::GetMatrix() const
{
	return Mat4x4::MakeAffin(scale, rotate, translate);
}

void Transform::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef USE_IMGUI
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("スケール", scale.data(), 0.01f);
	ImGui::DragFloat3("回転", rotate.data(), 0.01f);
	ImGui::DragFloat3("ポジション", translate.data(), 0.01f);
	ImGui::End();
#endif // USE_IMGUI
}

Mat4x4 QuaternionTransform::GetMatrix() const
{
	return Mat4x4::MakeAffin(scale, rotate, translate);
}
