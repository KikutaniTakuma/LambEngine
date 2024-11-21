#pragma once
#include "Math/Matrix.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/MathCommon.h"
#include "Math/Quaternion.h"
#include "Utils/SafePtr.h"

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

	void Init(const Vector3& min, const Vector3& max, float height);

	void Update();

	bool IsCollision(const Fish& other) const;


	void CalcAvoidDirection(const Fish& other);
	// 重心と平均方向を求めるために足す
	void AddAvgAndCenterOfGravityDirection(const Fish& other);
	// 重心と平均方向を求める
	void CalcAvgAndCenterOfGravityDirection();

	// 最終的な方向を決める
	void CalcDirection();

	Mat4x4 CreateWorldMatrix() const;

	uint32_t GetColors() const;

private:
	void CalcAvoidWallDirection(const Vector3& min, const Vector3& max);

public:
	// どれを優先にするかの重み
	Vector3 weight = {0.005f, 1.0f, 0.05f};
	// 視野範囲
	float fov = 45.0f * Lamb::Math::toRadian<float>;
	// 離れる距離
	float avoidDistance = 20.0f;
	// 当たり判定をする距離
	float collisionRange = 50.0f;


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

	float wallCollisionRange_ = 10.0f;


	float collisionCount_ = 0.0f;
	float minNearLength_ = std::numeric_limits<float>::max();

	Vector3 minRange_;
	Vector3 maxRange_;

	uint32_t color_;
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

	void Debug();

public:
	float height = -3.0f;

private:
	Vector3 rangeMin_;
	Vector3 rangeMax_;

private:
	std::vector<std::unique_ptr<Fish>> fishes_;
	Lamb::SafePtr<class Model> model_;
	Vector3 weight_ = { 0.005f, 1.0f, 0.005f };
	float avoidDistance_ = 5.0f;
	float fov_ = 30.0f * Lamb::Math::toRadian<float>;
	float collisionRange_ = 10.0f;
};