#pragma once
#include "Drawers/Model/Model.h"
#include <memory>

class Bullet {
public:
	Bullet() = default;
	~Bullet() = default;

	Bullet(const Bullet&) = delete;
	Bullet(Bullet&&) = delete;
	Bullet& operator=(const Bullet&) = delete;
	Bullet& operator=(Bullet&&) = delete;

public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw(const class Camera& camera);

	// 調整
	void Debug(const std::string& guiName);

	bool CollisionBullet(const Bullet& bullet);
	bool CollisionBullet(const Vector3& pos, float radius);

public:
	void SetStatus(
		const Vector3& pos,
		const Vector3& moveDirection,
		float speed,
		float attack,
		uint32_t color,
		float radius = 1.0f
	);

	void Enable() {
		isActive_ = true;
	}

	void Unenable() {
		isActive_ = false;
	}

public:
	bool GetIsActive() const {
		return isActive_;
	}

	float GetAttack() const {
		return attack_;
	}

private:
	// モデル
	std::unique_ptr<Model> model_;

	// 方向
	Vector3 moveDirection_;
	// 速度
	float speed_;

	// 攻撃力
	float attack_;

	// 半径
	float radius_;

	bool isActive_;
};