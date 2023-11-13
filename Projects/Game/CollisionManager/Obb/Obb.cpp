#include "Obb.h"
#include <numbers>
#include "externals/imgui/imgui.h"
#include "Game/CollisionManager/Plane/Plane.h"

Obb::Obb():
	center_(),
	scale_(Vector3::identity),
	rotate_(),
	worldMatrix_(),
	color_(std::numeric_limits<uint32_t>::max()),
	orientations_{
		Vector3::xIdy,
		Vector3::yIdy,
		Vector3::zIdy
	},
	lines_{},
	orientationLines_{}
{}

bool Obb::IsCollision(Vector3 pos, float radius) {
	std::array<Vector3, 3> orientations = {
		orientations_[0] * MakeMatrixRotate(rotate_),
		orientations_[1] * MakeMatrixRotate(rotate_),
		orientations_[2] * MakeMatrixRotate(rotate_)
	};

	std::array<Vector3, 2> positions = {
		Vector3(-size_), // 左下手前
		
		Vector3(+size_) // 右上奥
	};

	for (auto& position : positions) {
		position *= MakeMatrixScalar(scale_);
	}

	pos *= MakeMatrixInverse(MakeMatrixAffin(Vector3::identity, rotate_, center_));

	//std::array<Plane, 6> planes = {
	//	Plane{-orientations[1].Normalize(), (center_ - orientations[1]).Length()}, //底面
	//	Plane{-orientations[0].Normalize(), (center_ - orientations[0]).Length()}, // 左面
	//	Plane{-orientations[2].Normalize(), (center_ - orientations[2]).Length()}, // 最前面
	//	Plane{+orientations[1].Normalize(), (center_ + orientations[1]).Length()}, //上面
	//	Plane{+orientations[0].Normalize(), (center_ + orientations[0]).Length()}, // 右面
	//	Plane{+orientations[2].Normalize(), (center_ + orientations[2]).Length()} // 最背面
	//};

	//for (auto& plane : planes) {

	//}

	Vector3 closestPoint = {
		std::clamp(pos.x, positions[0].x,positions[1].x),
		std::clamp(pos.y, positions[0].y,positions[1].y),
		std::clamp(pos.z, positions[0].z,positions[1].z)
	};

	float distance = (closestPoint - pos).Length();

	if (distance <= radius) {
		isCollision_ = true;
		color_ = 0xff0000ff;
		return true;
	}
	else {
		isCollision_ = false;
		color_ = std::numeric_limits<uint32_t>::max();
		return false;
	}
}

void Obb::Update() {
	isCollision_.Update();

	worldMatrix_.Affin(scale_, rotate_, center_);

	size_ = scale_ * 0.5f;

	std::array<Vector3, 8> positions = {
		Vector3(-size_), // 左下手前
		Vector3(-size_.x, -size_.y, +size_.z), // 左下奥
		Vector3(+size_.x, -size_.y, -size_.z), // 右下手前
		Vector3(+size_.x, -size_.y, +size_.z), // 右下奥

		Vector3(-size_.x, +size_.y, -size_.z), // 左上手前
		Vector3(-size_.x, +size_.y, +size_.z), // 左上奥
		Vector3(+size_.x, +size_.y, -size_.z), // 右上手前
		Vector3(+size_) // 右上奥
	};

	lines_[0].start_ = positions[0];
	lines_[0].end_ = positions[1];

	lines_[1].start_ = positions[0];
	lines_[1].end_ = positions[2];

	lines_[2].start_ = positions[0];
	lines_[2].end_ = positions[4];


	lines_[3].start_ = positions[3];
	lines_[3].end_ = positions[1];

	lines_[4].start_ = positions[3];
	lines_[4].end_ = positions[2];

	lines_[5].start_ = positions[3];
	lines_[5].end_ = positions[7];


	lines_[6].start_ = positions[5];
	lines_[6].end_ = positions[4];

	lines_[7].start_ = positions[5];
	lines_[7].end_ = positions[7];

	lines_[8].start_ = positions[5];
	lines_[8].end_ = positions[1];


	lines_[9].start_ = positions[6];
	lines_[9].end_ = positions[4];

	lines_[10].start_ = positions[6];
	lines_[10].end_ = positions[7];

	lines_[11].start_ = positions[6];
	lines_[11].end_ = positions[2];

	for (size_t i = 0llu; i < orientationLines_.size(); i++) {
		orientationLines_[i].start_ = center_;
		orientationLines_[i].end_ = (orientations_[i] * size_[i]) * worldMatrix_;
	}

	for (auto& line : lines_) {
		line.start_ *= worldMatrix_;
		line.end_ *= worldMatrix_;
	}
}

void Obb::Draw(const Mat4x4& viewProjection) {
	for (auto& line : lines_) {
		line.Draw(viewProjection, color_);
	}

	for (size_t i = 0llu; i < orientationLines_.size(); i++) {
		orientationLines_[i].Draw(viewProjection, Vector4ToUint(Vector4{ orientations_[i], 1.0f}));
	}
}

void Obb::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("center", &center_.x, 0.01f);
	ImGui::DragFloat3("size", &scale_.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate_.x, 0.01f);
	static Vector4 colorEdit;
	colorEdit = UintToVector4(color_);
	ImGui::ColorEdit4("color", colorEdit.m.data());
	color_ = Vector4ToUint(colorEdit);
	ImGui::End();
#endif // _DEBUG
}