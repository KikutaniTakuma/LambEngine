#pragma once
#include "Drawers/AnimationModel/AnimationModel.h"
#include "Utils/SafePtr.h"
#include "Utils/Flg.h"
#include "Game/CollisionManager/Obb/Obb.h"
#include <memory>

class Player {
public:
	Player() = default;
	Player(const Player&) = default;
	Player(Player&&) = default;
	~Player() = default;

	Player& operator=(const Player&) = default;
	Player& operator=(Player&&) = default;

public:
	void Init(const Transform& transform);

	void Update();

	void AfterCollisionUpdate(const Vector3& pushVector);

	void Draw(const class Camera& camera);

	const Lamb::Flg& GetIsPunch() const {
		return isPunch_;
	}

public:
	// パンチフラグをtrueにしてモーションを再生する
	void Punch();

	// 移動
	void Move();

	// ジャンプ
	void Jump();

	void Falling();

	// 着地
	void Landing(bool isCollision);

	const Vector3& GetTranslate() const {
		return transform_.translate;
	}
	const Vector3& GetScale() const {
		return transform_.scale;
	}

	void SetIsCollision(bool isCollision) {
		isCollision_ = isCollision;
	}

	Obb& GetObb() {
		if (not obb_) {
			throw Lamb::Error::Code<Obb>("obb is nullptr(must call Init)", ErrorPlace);
		}
		return *obb_;
	}

private:
	void JumpReset();

private:
	ObbPtr obb_;

	// ブロックのモデル
	std::unique_ptr<AnimationModel> model_;

	// アニメーション
	Animations* punchAnimatons_;
	Animations* waitAnimatons_;
	Animations* walkAnimatons_;
	Animations* floatingAnimatons_;

	QuaternionTransform transform_;

	// パンチしたか
	Lamb::Flg isPunch_;

	float gravity_ = 0.0f;

	float jump_ = 0.0f;
	float jumpSpeed_ = 0.0f;
	Lamb::Flg isJump_;
	float jumpTime_ = 0.0f;

	Lamb::Flg isCollision_;

	float speed_ = 0.0f;
	Vector2 direction_;
	Vector2 playerDirection_ = Vector2::kYIdentity;
};