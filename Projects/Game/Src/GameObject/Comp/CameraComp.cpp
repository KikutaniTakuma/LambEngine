#include "CameraComp.h"
#include "Input/Input.h"

void CameraComp::Init() {
	transform_ = object_.AddComp<decltype(transform_)::type>();
}

void CameraComp::LastUpdate() {
	viewMatrix_ = transform_->GetWorldMatrix().Inverse();
	cameraMatrix_ = viewMatrix_ * ndcMatrix_;
}

void CameraComp::Save(nlohmann::json& json)
{
	SaveCompName(json);
}

void CameraComp::Load([[maybe_unused]]nlohmann::json& json)
{
}

const Mat4x4& CameraComp::GetViewMatrix() const
{
	return viewMatrix_;
}

const Mat4x4& CameraComp::GetToNdcMatrix() const
{
	return ndcMatrix_;
}

TransformComp& CameraComp::GetTransformComp() {
	return *transform_;
}

const TransformComp& CameraComp::GetTransformComp() const {
	return *transform_;
}

void CameraComp::SetNdcMatix(const Mat4x4& ndc) {
	ndcMatrix_ = ndc;
}

void CameraComp::Debug([[maybe_unused]] const std::string& guiName) {
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
				transform_->translate.x += mouseVelocity.x * translateSpeed_ * object_.GetDeltaTime() * translateSigned;
				transform_->translate.y += mouseVelocity.y * translateSpeed_ * object_.GetDeltaTime() * translateSigned;
			}

			if (not isOnImGui && not isShift && isMiddle) {
				eulerRotate_ = Vector3::kZero;
				eulerRotate_.x -= mouseVelocity.y * rotateSpeed_ * object_.GetDeltaTime() * rotateSigned;
				eulerRotate_.y += mouseVelocity.x * rotateSpeed_ * object_.GetDeltaTime() * rotateSigned;
				transform_->rotate *= Quaternion::EulerToQuaternion(eulerRotate_);
			}

			float isSigned = mouse->GetWheelVelocity();
			if (not (isSigned == 0.0f) && not isOnImGui) {
				transform_->translate += (Vector3::kZIdentity * isSigned) * transform_->rotate * object_.GetDeltaTime();
			}
		}

		ImGui::TreePop();
	}
#endif // _DEBUG
}
