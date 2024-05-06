#include "Obb.h"
#include <numbers>
#include <algorithm>
#include "imgui.h"
#include "Game/CollisionManager/Plane/Plane.h"
#include "Math/Quaternion.h"

Obb::Obb():
	center(),
	scale(Vector3::kIdentity),
	rotate(),
	color_(std::numeric_limits<uint32_t>::max()),
	localPositions_(),
	positions_(),
	localOrientations_(),
	orientations_()
{
	localPositions_ = std::make_unique<std::array<Vector3, 8>>();
	positions_ = std::make_unique<std::array<Vector3, 8>>();

	Vector3 basisLocal = Vector3::kIdentity * 0.5f;
	*localPositions_ = {
		Vector3(-basisLocal), // 左下手前
		Vector3(-basisLocal.x, -basisLocal.y, +basisLocal.z), // 左下奥
		Vector3(+basisLocal.x, -basisLocal.y, -basisLocal.z), // 右下手前
		Vector3(+basisLocal.x, -basisLocal.y, +basisLocal.z), // 右下奥

		Vector3(-basisLocal.x, +basisLocal.y, -basisLocal.z), // 左上手前
		Vector3(-basisLocal.x, +basisLocal.y, +basisLocal.z), // 左上奥
		Vector3(+basisLocal.x, +basisLocal.y, -basisLocal.z), // 右上手前
		Vector3(+basisLocal) // 右上奥
	};



	localOrientations_ = std::make_unique<std::array<Vector3, 3>>();
	orientations_ = std::make_unique<std::array<Vector3, 3>>();

	*localOrientations_ = {
		Vector3::kXIdentity,
		Vector3::kYIdentity,
		Vector3::kZIdentity
	};

}

bool Obb::IsCollision(Vector3 pos, float radius) {
	constexpr int32_t min = 0;
	constexpr int32_t max = 1;

	std::array<Vector3, 2> positions = {
		scale * 0.5f, // 左下手前
		
		-scale * 0.5f // 右上奥
	};

	pos *= Mat4x4::MakeAffin(Vector3::kIdentity, rotate, center).Inverse();

	Vector3 closestPoint = {
		std::clamp(pos.x, positions[min].x,positions[max].x),
		std::clamp(pos.y, positions[min].y,positions[max].y),
		std::clamp(pos.z, positions[min].z,positions[max].z)
	};

	float distance = (closestPoint - pos).Length();

	if (distance <= radius) {
		isCollision_ = true;
		color_ = 0xff0000ff;
		return true;
	}
	
	return false;
}

bool Obb::IsCollision(Obb& other) {
	// 分離軸(面法線)
	float length = 0.0f, otherLength = 0.0f;
	float min = 0.0f, max = 0.0f, otherMin = 0.0f, otherMax = 0.0f;

	std::array<float, 8> projectLength;
	std::array<float, 8> otherProjectLength;

	auto collsion = [&](const Vector3& separationAxis, float& sumSpan, float& longSpan)->void {
		for (size_t i = 0; i < 8llu; i++) {
			projectLength[i] = positions_->at(i).Dot(separationAxis);
			otherProjectLength[i] = other.positions_->at(i).Dot(separationAxis);
		}

		min = *std::min_element(projectLength.begin(), projectLength.end());
		max = *std::max_element(projectLength.begin(), projectLength.end());

		otherMin = *std::min_element(otherProjectLength.begin(), otherProjectLength.end());
		otherMax = *std::max_element(otherProjectLength.begin(), otherProjectLength.end());

		length = max - min;
		otherLength = otherMax - otherMin;

		sumSpan = length + otherLength;
		longSpan = std::max(max, otherMax) - std::min(min, otherMin);
	};

	for (const auto& separationAxis : *orientations_) {
		float sumSpan = 0.0f, longSpan = 0.0f;

		collsion(separationAxis, sumSpan, longSpan);

		if (sumSpan < longSpan) {
			return false;
		}
	}
	for (const auto& separationAxis : *(other.orientations_)) {
		float sumSpan = 0.0f, longSpan = 0.0f;

		collsion(separationAxis, sumSpan, longSpan);

		if (sumSpan < longSpan) {
			return false;
		}
	}

	for (const auto& orientation : *orientations_) {
		for (const auto& otherOrientation : *other.orientations_) {
			float sumSpan = 0.0f, longSpan = 0.0f;

			collsion(orientation.Cross(otherOrientation), sumSpan, longSpan);

			if (sumSpan < longSpan) {
				return false;
			}
		}
	}

	isCollision_ = true;
	color_ = 0xff0000ff;

	other.isCollision_ = true;
	other.color_ = 0xff0000ff;

	return true;
}

void Obb::Update() {
	Mat4x4&& worldMatrix = Mat4x4::MakeAffin(scale, rotate, center);

	for (size_t i = 0; i < localPositions_->size(); i++) {
		positions_->at(i) = localPositions_->at(i) * worldMatrix;
	}

	for (size_t i = 0; i < localOrientations_->size(); i++) {
		orientations_->at(i) = localOrientations_->at(i) * 0.5f * worldMatrix;
	}

	isCollision_ = false;
	color_ = std::numeric_limits<uint32_t>::max();
}

void Obb::Draw(const Mat4x4& viewProjection) {
	Line::Draw(
		(*positions_)[0],
		(*positions_)[1],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[0],
		(*positions_)[2],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[0],
		(*positions_)[4],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[3],
		(*positions_)[1],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[3],
		(*positions_)[2],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[3],
		(*positions_)[7],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[5],
		(*positions_)[4],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[5],
		(*positions_)[7],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[5],
		(*positions_)[1],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[6],
		(*positions_)[4],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[6],
		(*positions_)[7],
		viewProjection,
		color_
	);

	Line::Draw(
		(*positions_)[6],
		(*positions_)[2],
		viewProjection,
		color_
	);

	for (size_t i = 0llu; i < orientations_->size(); i++) {
		Line::Draw(
			center,
			orientations_->at(i),
			viewProjection,
			Vector4{ localOrientations_->at(i), 1.0f }.GetColorRGBA()
		);
	}
}

void Obb::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("center", center.data(), 0.01f);
	ImGui::DragFloat3("size", scale.data(), 0.01f);
	ImGui::DragFloat3("rotate", rotate.data(), 0.01f);
	static Vector4 colorEdit;
	colorEdit = UintToVector4(color_);
	ImGui::ColorEdit4("color", colorEdit.m.data());
	color_ = Vector4ToUint(colorEdit);
	ImGui::End();
#endif // _DEBUG
}