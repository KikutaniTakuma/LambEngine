#pragma once
#include "Drawers/Model/Model.h"
#include "Engine/Graphics/Animator/Animator.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Utils/Flg/Flg.h"
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
			throw Lamb::Error::Code<Obb>("obb is nullptr(must call Init)", __func__, __FILE__, __LINE__);
		}
		return *obb_;
	}

private:
	void JumpReset();

private:
	ObbPtr obb_;

	// ブロックのモデル
	Lamb::SafePtr<Model> model_;

	// アニメーション
	std::unique_ptr<Animator> punchAnimator_;
	std::unique_ptr<Animator> waitAnimator_;

	Transform transform_;

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
};