#pragma once
#include "Math/Matrix.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/MathCommon.h"
#include "Math/Quaternion.h"

#include <memory>

class Fish {
public:
	Fish() = default;
	Fish(const Fish&) = default;
	Fish(Fish&&) = default;
	~Fish() = default;

	Fish& operator=(const Fish&) = default;
	Fish& operator=(Fish&&) = default;

public:
	const Vector3& GetDirection() const;

	void Init(const Vector3& min, const Vector3& max);

	void Update();

	bool IsCollision(const Fish& other) const;


	void CalcAvoidWallDirection(const Vector3& min, const Vector3& max);
	void CalcAvoidDirection(const Fish& other);
	// 重心と平均方向を求めるために足す
	void AddAvgAndCenterOfGravityDirection(const Fish& other);
	// 重心と平均方向を求める
	void CalcAvgAndCenterOfGravityDirection();

	// 最終的な方向を決める
	void CalcDirection();

	Mat4x4 CreateWorldMatrix() const;


private:
	Vector3 posision_;
	Quaternion rotate_;

	float speed_ = 0.0f;

	// 最終的な移動方向
	Vector3 direction_;

	// 近くの人を避ける方向
	Vector3 avoidDirection_;

	// 近くの人の平均的な方向
	Vector3 avgDirection_;

	// 近くの人の重心の方向
	Vector3 centerOfGravityDirection_;

	// 壁から離れる
	Vector3 avoidWallDirection_;

	// 視野範囲
	float fov_ = 45.0f * Lamb::Math::toRadian<float>;

	// 当たり判定をする距離
	float collisionRange_ = 0.0f;

	float wallCollisionRange_ = 0.0f;

	// どれを優先にするかの重み
	Vector3 weight_ = Vector3::kXIdentity;

	float collisionCount_ = 0.0f;
	float minNearLength = std::numeric_limits<float>::max();
};

class Fishes {
public:
	Fishes() = default;
	Fishes(const Fishes&) = default;
	Fishes(Fishes&&) = default;
	~Fishes() = default;

	Fishes& operator=(const Fishes&) = default;
	Fishes& operator=(Fishes&&) = default;

public:
	void Init(size_t numFishes);

	void Update();

	void Draw(const Mat4x4& cameraMat);

private:
	Vector3 rangeMin_;
	Vector3 rangeMax_;

private:
	std::vector<std::unique_ptr<Fish>> fishes_;
	Lamb::SafePtr<class Model> model_;
};