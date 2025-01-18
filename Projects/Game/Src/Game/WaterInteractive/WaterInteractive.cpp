#include "WaterInteractive.h"
#include "Game/Water/Water.h"

#include "Utils/Easeing.h"

#ifdef USE_DEBUG_CODE
#include "imgui.h"
#include "Drawer/Line/Line.h"
#endif // USE_DEBUG_CODE


WaterInteractive::WaterInteractive() :
	pWater_(Water::GetInstance()),
	worldMatrix_(Mat4x4::kIdentity),
	dir_(Quaternion::kIdentity),
	pre_(Quaternion::kIdentity),
	next_(Quaternion::kIdentity)
{
	nextTime_ = 1.5f;
	count_ = nextTime_;
}

void WaterInteractive::Update() {
	const Mat4x4&& waterMatrixInverse = pWater_->transform.CreateMatrix().Inverse();

	if (nextTime_ <= count_) {
		quad_ = Quad::CalcWorldPositions(worldMatrix_);
		Vector4 heights;
		preHeight_ = nextHeight_;
		nextHeight_ = 0.0f;

		for (size_t index = 0; auto & i : quad_.positions) {
			Vector3 uv = Vector3(i.x, 0.f, i.z) * waterMatrixInverse;
			heights[index] = pWater_->CalcWaveHeight(Vector2(uv.x, uv.z));
			nextHeight_ += heights[index];
			index++;
		}

		nextHeight_ *= 0.25f;

		float32_t up = (heights[0] + heights[1]) * 0.5f;
		float32_t down = (heights[2] + heights[3]) * 0.5f;
		float32_t right = (heights[1] + heights[2]) * 0.5f;
		float32_t left = (heights[0] + heights[3]) * 0.5f;

		float32_t yx = (right - left) / (quad_.positions[0] - quad_.positions[3]).Length();
		float32_t yz = (up - down) / (quad_.positions[1] - quad_.positions[0]).Length();

		Vector3 normal = Vector3(-yx, 1.0f, -yz).Normalize();

		pre_ = next_;
		next_ = Quaternion::DirectionToDirection(Vector3::kYIdentity, normal);

		count_ = 0.0f;
	}

	float t = Easeing::InOutSine(count_ / nextTime_);
	dir_ = Quaternion::Slerp(pre_, next_, t);
	height_ = std::lerp(preHeight_, nextHeight_, t);

#ifdef USE_DEBUG_CODE
	debugNormal_ = Vector3::kYIdentity * dir_;
#endif // USE_DEBUG_CODE

	count_ += Lamb::DeltaTime();
}

void WaterInteractive::SetMatrix(const Mat4x4& worldMatrix) {
	worldMatrix_ = worldMatrix;
}

void WaterInteractive::Debug([[maybe_unused]] const Mat4x4& camera, [[maybe_unused]] const std::string& guiName) {
#ifdef USE_DEBUG_CODE
	if (ImGui::TreeNode(guiName.c_str())) {
		ImGui::DragFloat("波の反応間隔(s):", &nextTime_, 0.01f, 0.0f, 2.0f);


		ImGui::TreePop();
	}

#endif // USE_DEBUG_CODE
}

float WaterInteractive::GetHeight() const
{
	return height_;
}

const Quaternion& WaterInteractive::GetQuaternion() const {
	return dir_;
}
