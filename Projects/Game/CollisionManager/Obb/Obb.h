#pragma once
#include "Alfort/Utils/Math/Vector3.h"
#include "Alfort/Utils/Math/Mat4x4.h"
#include "Alfort/Drawers/Line/Line.h"
#include "Alfort/Utils/UtilsLib/UtilsLib.h"
#include <array>

/// <summary>
/// 当たり判定(OBB)
/// </summary>
class Obb {
public:
	Obb();
	Obb(const Obb&) = default;
	Obb(Obb&&) = default;
	~Obb() = default;

	Obb& operator=(const Obb&) = default;
	Obb& operator=(Obb&&) = default;

public:
	void Draw(const Mat4x4& viewProjection);

	void Update();

	void Debug(const std::string& guiName);

	bool IsCollision(Vector3 pos, float radius);

public:
	Vector3 center_;
	Vector3 scale_;
	Vector3 rotate_;

	uint32_t color_;

private:
	std::array<Vector3, 3> orientations_;
	std::array<Line, 12> lines_;
	std::array<Line, 3> orientationLines_;
	Vector3 size_;

	Mat4x4 worldMatrix_;

	UtilsLib::Flg isCollision_;

/// <summary>
/// ゲッター
/// </summary>
public:
	const UtilsLib::Flg& GetIsCollision() const {
		return isCollision_;
	}
};