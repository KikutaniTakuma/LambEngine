#pragma once
#include "Drawers/Model/Model.h"
#include "../Bullet/Bullet.h"
#include <memory>

class Player {
public:
	Player() = default;
	~Player() = default;

	Player(const Player&) = delete;
	Player(Player&&) = delete;
	Player& operator=(const Player&) = delete;
	Player& operator=(Player&&) = delete;

public:
	// 初期化
	void Initialize();

	// 移動
	void Move();

	// 更新
	void Update();

	// 描画
	void Draw(const class Camera& camera);

	// 調整
	void Debug(const std::string& guiName);

	void Collision(const class Enemy& enemy);

public:
	inline const Vector3& GetPos() const {
		return model_->pos;
	}

	inline float GetRotate() const {
		return rotate_;
	}

	inline bool IsGameOver() const {
		return hp_ < 0.0f;
	}

	inline const std::list<std::unique_ptr<class Bullet>> GetBullets() const {
		return bullets_;
	}

private:
	// モデル
	std::unique_ptr<Model> model_;

	// 最大速度
	float maxSpeed_;
	// 速度加算
	float addSpeed_;
	// 速度
	float speed_;
	// 速度減衰
	float speedDecay_;

	// 回転
	float rotate_;


	// 真ん中からのoffset
	Vector3 offset_;
	// オフセットの移動速度
	float offsetSpeed_;
	// オフセットの最大
	Vector3 maxOffset_;
	// オフセットの最小
	Vector3 minOffset_;

	// オフセットの長さを元にしたスピードの倍率
	float speedScale_;

	// オフセットの基準
	float basisSpeedScale_;


	// 攻撃値
	float attack_;

	// 弾(6発)
	std::list<std::unique_ptr<class Bullet>> bullets_;

	// 当たり判定で使う
	float radius_;

	// 当たったか
	bool isCollisioned_;
	// 無敵時間(秒)
	float invincibleTime_;
	// 当たってからの時間(秒)
	float isCollisionedTime_;

	// 体力
	float hp_;
};