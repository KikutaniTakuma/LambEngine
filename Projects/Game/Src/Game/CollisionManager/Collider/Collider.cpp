#include "Collider.h"

#include <algorithm>
#include <cmath>

#include "imgui.h"

void Collider::InitializeCollision(const Vector3& size, const Vector3& rotate, const Vector3& pos) {
	aabbMin_.emplace_back(Vector3());
	aabbMax_.emplace_back(Vector3());
	obbSize_.emplace_back(Vector3());
	aabb_.emplace_back(AABB());
	obb_.emplace_back(OBB());
	SetCollisionScale(size, uint32_t(aabbMin_.size()) - 1);
	UpdateCollision(rotate, pos, uint32_t(aabbMin_.size()) - 1);
	color_.emplace_back(Vector4ToUint(Vector4::kIdentity));
	std::array<std::unique_ptr<Line>, 12> tmp{};
	for (auto& i : tmp) {
		i = std::make_unique<Line>();
	}
	lines_.emplace_back(std::move(tmp));
}

void Collider::UpdateCollision(const Vector3& rotate, const Vector3& pos, uint32_t index) {
	aabb_.at(index) = {
		.center{pos},
		.min{pos + aabbMin_.at(index) },
		.max{pos + aabbMax_.at(index) },
	};

	obb_.at(index) = {
		.center{pos},
		.orientations{
				 {1.0f, 0.0f, 0.0f},
				 {0.0f, 1.0f, 0.0f},
				 {0.0f, 0.0f, 1.0f},
				 },
		.size{obbSize_.at(index) },
	};

	obb_.at(index).SetRotate(rotate);
}

void Collider::DebugDraw(const Mat4x4& viewProjection, uint32_t index) {
	std::vector<Vector3> positions = obb_.at(index).GetVertices();

	for (size_t i = 0; i < 4; i++) {
		size_t j = (i + 1) % 4;
		lines_.at(index)[i]->start = positions[i];
		lines_.at(index)[i]->end = positions[j];

		lines_.at(index)[i + 4]->start = positions[i];
		lines_.at(index)[i + 4]->end = positions[i + 4];

		lines_.at(index)[i + 8]->start = positions[i + 4];
		lines_.at(index)[i + 8]->end = positions[j + 4];
	}

	for (auto& line : lines_) {
		for (auto& l : line) {
		l->color = color_.at(index);
		l->Draw(viewProjection);
		}
	}
	//positions = {
	//	// 左下手前
	//	Vector3(aabb_.at(index).min.x, aabb_.at(index).min.y, aabb_.at(index).min.z),
	//	// 左下奥
	//	Vector3(aabb_.at(index).min.x, aabb_.at(index).min.y, aabb_.at(index).max.z),
	//	// 右下手前
	//	Vector3(aabb_.at(index).max.x, aabb_.at(index).min.y, aabb_.at(index).min.z),
	//	// 右下奥
	//	Vector3(aabb_.at(index).max.x, aabb_.at(index).min.y, aabb_.at(index).max.z),

	//	// 左上手前
	//	Vector3(aabb_.at(index).min.x, aabb_.at(index).max.y, aabb_.at(index).min.z),
	//	// 左上奥
	//	Vector3(aabb_.at(index).min.x, aabb_.at(index).max.y, aabb_.at(index).max.z),
	//	// 右上手前
	//	Vector3(aabb_.at(index).max.x, aabb_.at(index).max.y, aabb_.at(index).min.z),
	//	// 右上奥
	//	Vector3(aabb_.at(index).max.x, aabb_.at(index).max.y, aabb_.at(index).max.z)
	//};

	//lines_.at(index)[0]->start = positions[0];
	//lines_.at(index)[0]->end = positions[1];

	//lines_.at(index)[1]->start = positions[0];
	//lines_.at(index)[1]->end = positions[2];

	//lines_.at(index)[2]->start = positions[0];
	//lines_.at(index)[2]->end = positions[4];


	//lines_.at(index)[3]->start = positions[3];
	//lines_.at(index)[3]->end = positions[1];

	//lines_.at(index)[4]->start = positions[3];
	//lines_.at(index)[4]->end = positions[2];

	//lines_.at(index)[5]->start = positions[3];
	//lines_.at(index)[5]->end = positions[7];


	//lines_.at(index)[6]->start = positions[5];
	//lines_.at(index)[6]->end = positions[4];

	//lines_.at(index)[7]->start = positions[5];
	//lines_.at(index)[7]->end = positions[7];

	//lines_.at(index)[8]->start = positions[5];
	//lines_.at(index)[8]->end = positions[1];


	//lines_.at(index)[9]->start = positions[6];
	//lines_.at(index)[9]->end = positions[4];

	//lines_.at(index)[10]->start = positions[6];
	//lines_.at(index)[10]->end = positions[7];

	//lines_.at(index)[11]->start = positions[6];
	//lines_.at(index)[11]->end = positions[2];

	//for (auto& line : lines_) {
	//	for (auto& l : line) {
	//		l->color = color_.at(index);
	//		l->Draw(viewProjection);
	//	}
	//}
}

void Collider::Debug([[maybe_unused]] const std::string& guiName, [[maybe_unused]] uint32_t index) {
#ifdef USE_IMGUI
	ImGui::Begin(guiName.c_str());

	ImGui::DragFloat3("OBBCenter", &obb_.at(index).center.x, 0.01f);
	ImGui::DragFloat3("OBBScale", &obbSize_.at(index).x, 0.01f, 0.001f, std::numeric_limits<float>::max());

	ImGui::End();
#endif // USE_IMGUI
}

void Collider::SetCollisionScale(const Vector3& size, uint32_t index) {
	Vector3 obbSize = size * 2.0f;
	Vector3 aabbSize{};

	aabbSize.x = std::sqrt((obbSize.x * 0.5f * obbSize.x * 0.5f) + obbSize.y * obbSize.y + obbSize.z * obbSize.z);
	aabbSize.y = std::sqrt(obbSize.x * obbSize.x + (obbSize.y * 0.5f * obbSize.y * 0.5f) + obbSize.z * obbSize.z);
	aabbSize.z = std::sqrt(obbSize.x * obbSize.x + obbSize.y * obbSize.y + (obbSize.z * 0.5f) * (obbSize.z * 0.5f));

	float aabbHalfSize = (std::max)((std::max)(aabbSize.x, aabbSize.y), aabbSize.z) * 0.5f;

	aabbMin_.at(index) = (Vector3{ -aabbHalfSize, -aabbHalfSize, -aabbHalfSize });
	aabbMax_.at(index) = (Vector3{ aabbHalfSize, aabbHalfSize, aabbHalfSize });

	obbSize_.at(index) = (obbSize);
}