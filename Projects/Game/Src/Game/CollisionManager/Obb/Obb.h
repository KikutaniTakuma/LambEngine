#pragma once
#include "Math/Vector3.h"
#include "Math/Mat4x4.h"
#include "Drawers/Line/Line.h"
#include "Utils/Flg/Flg.h"
#include <array>
#include <memory>

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

	[[nodiscard]]bool IsCollision(Vector3 pos, float radius);
	[[nodiscard]]bool IsCollision(Obb& other);
	[[nodiscard]]bool IsCollision(Obb& other, Vector3& pushVector);

public:
	Vector3 center;
	Vector3 scale;
	Vector3 rotate;

private:
	uint32_t color_;

private:
	std::unique_ptr<std::array<Vector3, 8>> localPositions_;
	std::unique_ptr<std::array<Vector3, 8>> positions_;
	std::unique_ptr<std::array<Vector3, 3>> localOrientations_;
	std::unique_ptr<std::array<Vector3, 3>> orientations_;

	Lamb::Flg isCollision_;

/// <summary>
/// ゲッター
/// </summary>
public:
	const Lamb::Flg& GetIsCollision() const {
		return isCollision_;
	}
};