#pragma once
#include <vector>

#include "../AABB/AABB.h"
#include "../Obb/Obb.h"
#include "Math/Mat4x4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace Collision {
	// AABBとAABB
	bool IsCollision(const AABB& a, const AABB& b);
	// OBBとOBB
	bool IsCollision(const OBB& obb1, const OBB& obb2);
	// めり込み
	bool IsCollision(const OBB& obb1, const OBB& obb2, Vector3& vector);
	bool IsCollision(const OBB& obb1, const OBB& obb2, Vector3& vector,const Vector3& pushVector);
	// 分離軸
	bool SeparationAxis(const Vector3& axis, const OBB& obb_1, const OBB& obb_2);
	bool SeparationAxis(const Vector3& axis, const OBB& obb_1, const OBB& obb_2,Vector3& minAxis, float& minOverlap);

	// OBBIndex
	void OBBIndex(const OBB& obb, std::vector<Vector3>& output_vertices);
	Mat4x4 SetRotate(const Vector3(&array)[3]);
	Vector2 Projection(const std::vector<Vector3>& vertices, const Vector3& axis);
	float GetOverlap(const Vector2& minmax1, const Vector2& minmax2);
}