#include "Camera3DComp.h"
#ifdef _DEBUG
#include "Input/Input.h"
#include "imgui.h"
#include "Utils/EngineInfo.h"
#endif // _DEBUG


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
	if (ImGui::TreeNode(guiName.c_str())) {
		ImGui::Checkbox("Debug Mode", &isDebug_);

		if (isDebug_) {
			if (ImGui::TreeNode("Debug UI Setting")) {
				ImGui::Checkbox("カメラ回転反転", &isRotateInverse_);
				ImGui::Checkbox("カメラ移動反転", &isTranslateInverse_);
				ImGui::DragFloat("ズーム速度", &scaleSpeed_, 0.01f);
				ImGui::DragFloat("回転速度", &rotateSpeed_, 0.01f);
				ImGui::DragFloat("移動速度", &translateSpeed_, 0.01f);
				ImGui::TreePop();
			}

			Mouse* const mouse = Mouse::GetInstance();
			KeyInput* const key = KeyInput::GetInstance();

			bool isOnImGui = ImGui::GetIO().WantCaptureMouse;
			bool isShift = key->LongPush(DIK_LSHIFT) || key->LongPush(DIK_RSHIFT);
			bool isMiddle = mouse->LongPush(Mouse::Button::Middle);
			const Vector2& mouseVelocity = mouse->GetVelocity();
			float rotateSigned = isRotateInverse_ ? 1.0f : -1.0f;
			float translateSigned = isTranslateInverse_ ? 1.0f : -1.0f;

			if (not isOnImGui && isShift && isMiddle) {
				transform_->translate.x += mouseVelocity.x * translateSpeed_ * Lamb::DeltaTime() * translateSigned;
				transform_->translate.y += mouseVelocity.y * translateSpeed_ * Lamb::DeltaTime() * translateSigned;
			}

			if (not isOnImGui && not isShift && isMiddle) {
				transform_->rotate *= Quaternion::MakeRotateXAxis(mouseVelocity.y * rotateSpeed_ * Lamb::DeltaTime() * rotateSigned);
				transform_->rotate *= Quaternion::MakeRotateYAxis(mouseVelocity.x * rotateSpeed_ * Lamb::DeltaTime() * rotateSigned);
			}

			float isSigned = mouse->GetWheelVelocity();
			if (not (isSigned == 0.0f) && not isOnImGui) {
				transform_->translate += (Vector3::kZIdentity * isSigned) * transform_->rotate * Lamb::DeltaTime();
			}
		}

		ImGui::TreePop();
	}
#endif // _DEBUG
}
