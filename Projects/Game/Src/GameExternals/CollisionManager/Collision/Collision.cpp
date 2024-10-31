#include "Collision.h"

#include "Math/Vector3.h"

#include <cmath>

bool Collision::IsCollision(const AABB& a, const AABB& b) {
	if ((a.min.x <= b.max.x && a.max.x >= b.min.x) && // x軸
		(a.min.y <= b.max.y && a.max.y >= b.min.y) && // y軸
		(a.min.z <= b.max.z && a.max.z >= b.min.z)    // z軸
		) {
		// 衝突
		return true;
	}
	return false;
}

bool Collision::IsCollision(const OBB& obb1, const OBB& obb2) {
	const int kAxis = 3;
	for (size_t i = 0; i < kAxis; i++) {
		// obb_1の軸
		if (SeparationAxis(obb1.orientations[i], obb1, obb2)) {
			return false;
		}
		// obb_2の軸
		if (SeparationAxis(obb2.orientations[i], obb1, obb2)) {
			return false;
		}
		for (size_t j = 0; j < kAxis; j++) {
			Vector3 tmp = obb1.orientations[i].Cross(obb2.orientations[j]);
			if (SeparationAxis(obb1.orientations[i], obb1, obb2)) {
				return false;
			}
		}
	}
	return true;
}

bool Collision::IsCollision(const OBB& obb1, const OBB& obb2, Vector3& outputPushVector) {
	std::vector<Vector3> vertices_1;
	std::vector<Vector3> vertices_2;
	// 配列に頂点を代入
	OBBIndex(obb1, vertices_1);
	OBBIndex(obb2, vertices_2);

	std::vector<Vector3> axes = {
		obb1.orientations[0],
		obb1.orientations[1],
		obb1.orientations[2],

		obb2.orientations[0],
		obb2.orientations[1],
		obb2.orientations[2],

		obb1.orientations[0].Cross(obb2.orientations[0]).Normalize(),
		obb1.orientations[0].Cross(obb2.orientations[1]).Normalize(),
		obb1.orientations[0].Cross(obb2.orientations[2]).Normalize(),

		obb1.orientations[1].Cross(obb2.orientations[0]).Normalize(),
		obb1.orientations[1].Cross(obb2.orientations[1]).Normalize(),
		obb1.orientations[1].Cross(obb2.orientations[2]).Normalize(),

		obb1.orientations[2].Cross(obb2.orientations[0]).Normalize(),
		obb1.orientations[2].Cross(obb2.orientations[1]).Normalize(),
		obb1.orientations[2].Cross(obb2.orientations[2]).Normalize(),
	};
	float minOverLap = FLT_MAX;
	Vector3 minOverlapAxis{};
	// 分離軸判定関数
	auto IsSeparateAxis = [&](const Vector3& axis) {
		if (axis == Vector3::kZero) { return false; }
		Vector2 minmax1 = Projection(vertices_1, axis);
		Vector2 minmax2 = Projection(vertices_2, axis);

		// 分離軸である
		if (!(minmax1.x <= minmax2.y && minmax1.y >= minmax2.x)) {
			return true;
		}

		float overlap = GetOverlap(minmax1, minmax2);

		if (overlap < minOverLap) {
			minOverlapAxis = axis;
			minOverLap = overlap;
		}

		return false;
		};

	for (auto & axis : axes) {
		if (std::isnan(axis.x) || std::isnan(axis.y) || std::isnan(axis.z)) { continue; }
		if (IsSeparateAxis(axis)) { return false; }
	}
	float dot = (obb2.center - obb1.center).Dot(minOverlapAxis.Normalize());
	if (dot > 0.0f) {
		minOverlapAxis *= -1.0f;
	}
	outputPushVector = minOverlapAxis.Normalize() * minOverLap;
	return true;
}

bool Collision::IsCollision(const OBB& obb1, const OBB& obb2, Vector3& outputPushVector, const Vector3& pushVector) {
	std::vector<Vector3> vertices_1;
	std::vector<Vector3> vertices_2;
	// 配列に頂点を代入
	OBBIndex(obb1, vertices_1);
	OBBIndex(obb2, vertices_2);

	std::vector<Vector3> axes = {
		obb1.orientations[0],
		obb1.orientations[1],
		obb1.orientations[2],

		obb2.orientations[0],
		obb2.orientations[1],
		obb2.orientations[2],

		obb1.orientations[0].Cross(obb2.orientations[0]).Normalize(),
		obb1.orientations[0].Cross(obb2.orientations[1]).Normalize(),
		obb1.orientations[0].Cross(obb2.orientations[2]).Normalize(),

		obb1.orientations[1].Cross(obb2.orientations[0]).Normalize(),
		obb1.orientations[1].Cross(obb2.orientations[1]).Normalize(),
		obb1.orientations[1].Cross(obb2.orientations[2]).Normalize(),

		obb1.orientations[2].Cross(obb2.orientations[0]).Normalize(),
		obb1.orientations[2].Cross(obb2.orientations[1]).Normalize(),
		obb1.orientations[2].Cross(obb2.orientations[2]).Normalize(),
	};
	pushVector;
	float minOverLap = FLT_MAX;
	Vector3 minOverlapAxis{};
	// 分離軸判定関数
	auto IsSeparateAxis = [&](const Vector3& axis) {
		if (axis == Vector3::kZero) { return false; }
		Vector2 minmax1 = Projection(vertices_1, axis);
		Vector2 minmax2 = Projection(vertices_2, axis);

		// 分離軸である
		if (!(minmax1.x <= minmax2.y && minmax1.y >= minmax2.x)) {
			return true;
		}

		float overlap = GetOverlap(minmax1, minmax2);

		if (overlap < minOverLap) {
			minOverlapAxis = axis;
			minOverLap = overlap;
		}

		return false;
		};

	for (auto& axis : axes) {
		if (std::isnan(axis.x) || std::isnan(axis.y) || std::isnan(axis.z)) { continue; }
		if (IsSeparateAxis(axis)) { return false; }
	}
	float dot = (obb2.center - obb1.center).Dot(minOverlapAxis.Normalize());
	if (dot > 0.0f) {
		minOverlapAxis *= -1.0f;
	}
	outputPushVector = minOverlapAxis.Normalize() * minOverLap;
	return true;
}

bool Collision::SeparationAxis(const Vector3& axis, const OBB& obb_1, const OBB& obb_2) {
	Vector3 L = axis;
	// 頂点数
	const int32_t kIndex = 8;
	// 頂点格納用配列
	std::vector<Vector3> vertices_1;
	std::vector<Vector3> vertices_2;
	// 配列に頂点を代入
	OBBIndex(obb_1, vertices_1);
	OBBIndex(obb_2, vertices_2);
	// 距離を格納
	float min_1 = vertices_1[0].Dot(L);
	float max_1 = min_1;
	float min_2 = vertices_2[0].Dot(L);
	float max_2 = min_2;
	for (size_t i = 1; i < kIndex; i++) {
		float dot_1 = vertices_1[i].Dot(L);
		float dot_2 = vertices_2[i].Dot(L);
		// min/max比べる
		min_1 = (std::min)(min_1, dot_1);
		max_1 = (std::max)(max_1, dot_1);
		min_2 = (std::min)(min_2, dot_2);
		max_2 = (std::max)(max_2, dot_2);
	}
	float L1 = max_1 - min_1;
	float L2 = max_2 - min_2;
	float sumSpan = L1 + L2;
	float longSpan = (std::max)(max_1, max_2) - (std::min)(min_1, min_2);
	// 分離軸である
	if (sumSpan < longSpan) {
		return true;
	}
	return false;
}

bool Collision::SeparationAxis(const Vector3& axis, const OBB& obb_1, const OBB& obb_2, Vector3& minAxis, float& minOverlap) {
	Vector3 L = axis;
	// 頂点数
	const int32_t kIndex = 8;
	// 頂点格納用配列
	std::vector<Vector3> vertices_1;
	std::vector<Vector3> vertices_2;
	// 配列に頂点を代入
	OBBIndex(obb_1, vertices_1);
	OBBIndex(obb_2, vertices_2);
	// 距離を格納
	float min_1 = vertices_1[0].Dot(L);
	float max_1 = min_1;
	float min_2 = vertices_2[0].Dot(L);
	float max_2 = min_2;
	for (size_t i = 1; i < kIndex; i++) {
		float dot_1 = vertices_1[i].Dot(L);
		float dot_2 = vertices_2[i].Dot(L);
		// min/max比べる
		min_1 = (std::min)(min_1, dot_1);
		max_1 = (std::max)(max_1, dot_1);
		min_2 = (std::min)(min_2, dot_2);
		max_2 = (std::max)(max_2, dot_2);
	}
	float L1 = max_1 - min_1;
	float L2 = max_2 - min_2;
	float sumSpan = L1 + L2;
	float longSpan = (std::max)(max_1, max_2) - (std::min)(min_1, min_2);
	// 分離軸である
	if (sumSpan < longSpan) {
		return true;
	}
	float overlap = sumSpan - longSpan;
	if (overlap < minOverlap) {
		minOverlap = overlap;
		minAxis = axis;
	}
	return false;
}

void Collision::OBBIndex(const OBB& obb, std::vector<Vector3>& output_vertices) {
	Vector3 halfSize = obb.size * 0.5f;
	std::vector<Vector3> vertices = {
			{-halfSize},
			{+halfSize.x, -halfSize.y, -halfSize.z},
			{+halfSize.x, -halfSize.y, +halfSize.z},
			{-halfSize.x, -halfSize.y, +halfSize.z},
			{-halfSize.x, +halfSize.y, -halfSize.z},
			{+halfSize.x, +halfSize.y, -halfSize.z},
			{+halfSize},
			{-halfSize.x, +halfSize.y, +halfSize.z},
	};

	Mat4x4 rotateMatrix = SetRotate(obb.orientations);
	for (auto& vertex : vertices) {
		vertex = vertex * rotateMatrix;
		vertex = vertex + obb.center;
	}
	output_vertices = vertices;
}

Mat4x4 Collision::SetRotate(const Vector3(&array)[3]) {
	Mat4x4 rotateMatrix;
	rotateMatrix = Mat4x4::kIdentity;
	rotateMatrix[0][0] = array[0].x;
	rotateMatrix[0][1] = array[0].y;
	rotateMatrix[0][2] = array[0].z;
	rotateMatrix[1][0] = array[1].x;
	rotateMatrix[1][1] = array[1].y;
	rotateMatrix[1][2] = array[1].z;
	rotateMatrix[2][0] = array[2].x;
	rotateMatrix[2][1] = array[2].y;
	rotateMatrix[2][2] = array[2].z;
	return rotateMatrix;
}

Vector2 Collision::Projection(const std::vector<Vector3>& vertices, const Vector3& axis) {
	Vector2 minmax(axis.Dot(vertices[0]), axis.Dot(vertices[0]));
	for (size_t i = 1; i < vertices.size(); ++i) {
		float dot = axis.Dot(vertices[i]);
		minmax.x = std::min(dot, minmax.x);
		minmax.y = std::max(dot, minmax.y);
	}
	return minmax;
}

float Collision::GetOverlap(const Vector2& minmax1, const Vector2& minmax2) {
	float range1 = minmax1.y - minmax1.x;
	float range2 = minmax2.y - minmax2.x;
	float maxOverlap = std::max(minmax1.y, minmax2.y) - std::min(minmax1.x, minmax2.x);
	return range1 + range2 - maxOverlap;
}
