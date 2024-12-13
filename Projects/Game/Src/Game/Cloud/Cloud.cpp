#include "Cloud.h"

#ifdef USE_DEBUG_CODE
#include "imgui.h"
#endif // USE_DEBUG_CODE


Cloud::Cloud() {
	cloudTex_ = std::make_unique<CloudTex2D>();
	cloudTex_->Load();

	transform_.scale = Vector3::kIdentity * 1000.0f;
	transform_.scale.z = 1.0f;

	transform_.translate.y = 40.0f;

	vec_.x = 60.119998931884766f;
	vec_.y = 0.459f;
	vec_.z = 1.0f;
}

void Cloud::Draw(const Mat4x4& camera)
{
#ifdef USE_DEBUG_CODE
	if (ImGui::TreeNode("Cloud")) {
		ImGui::DragFloat3("scale", transform_.scale.data());
		ImGui::DragFloat3("transform", transform_.translate.data());

		ImGui::TreePop();
	}
#endif // USE_DEBUG_CODE


	uvTransform_.translate.x += 0.01f * Lamb::DeltaTime();
	uvTransform_.translate.y += 0.01f * Lamb::DeltaTime();

	cloudTex_->Draw(
		transform_.GetMatrix(),
		uvTransform_.GetMatrix(),
		camera,
		0u,
		vec_,
		0xffffffff,
		BlendType::kNormal
	);
}
