#pragma once
#include "Drawers/Model/Model.h"
#include "Utils/SafePtr/SafePtr.h"
#include "../Bullet/Bullet.h"
#include "Drawers/Particle/Particle.h"
#include "./Camera/Camera.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Transform/Transform.h"
#include <memory>
#include <array>

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
	void Update(const class Camera& camera);

	// 描画
	void Draw(const class Camera& camera);

	void AfterDraw();
	// 調整
	void Debug(const std::string& guiName);

	bool Collision(const class Enemy& enemy);

	void Attack(const class Enemy& enemy);

private:
	void CreateBullets();


public:
	inline const Vector3& GetPos() const {
		return noUpDownPos_;
	}

	inline float GetRotate() const {
		return rotate_;
	}

	// 終わった瞬間を取得
	inline bool IsGameOver() const {
		return hp_ < 0.0f && !particle_->GetIsParticleStart();
	}

	inline float GetHp() const {
		return hp_;
	}

	inline const std::array<std::unique_ptr<Bullet>,6>& GetBullets() const {
		return bullets_;
	}

private:
	// モデル
	Lamb::SafePtr<Model> model_;
	Transform transform_;
	uint32_t color_;

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
	std::array<std::unique_ptr<Bullet>,6> bullets_;
	std::array<bool, 6> isDrawbulletsUI_;
	bool isReloadable_;


	size_t currentBullet_;

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


	// 動いてるときのパーティクル
	std::unique_ptr<Particle> particle_;
	//パーティクル用カメラ
	std::unique_ptr<Camera> particleCamera_;
	// パーティクルを行ったか
	bool isParticled_;

	// モデルをゆらゆらさせる
	float modelUpDown_;
	// モデルをゆらゆらさせる速度
	float modelUpDownSpeed_;

	//ゆらゆらさせてないポジション
	Vector3 noUpDownPos_;

	//std::unique_ptr<Camera> uiCamera_;
	/*std::unique_ptr<Texture2D> uiFrame_;
	std::unique_ptr<Texture2D> uiHp_;
	std::unique_ptr<Texture2D> uiBulletFrame_;*/
	//std::array<std::unique_ptr<Texture2D>, 6> uiBullet_;

	float chikachika_;
};