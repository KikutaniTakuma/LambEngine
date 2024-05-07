#pragma once
#include "Transform/Transform.h"
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
	Transform transform;

#ifdef _DEBUG
private:
	uint32_t color_;
#endif // _DEBUG

private:
	static std::unique_ptr<std::array<const Vector3, 8>> localPositions_;
	static std::unique_ptr<std::array<const Vector3, 3>> localOrientations_;
private:
	std::unique_ptr<std::array<Vector3, 8>> positions_;
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

using ObbPtr = std::unique_ptr<Obb>;

template<class... Types>
ObbPtr MakeObb(Types&&... args) {
	return std::make_unique<Obb>(std::forward<Types>(args)...);
}