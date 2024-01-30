#pragma once
#include "Drawers/Model/Model.h"
#include "../Bullet/Bullet.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/Texture2D/Texture2D.h"
#include <memory>
#include <array>
#include <list>
#include <unordered_map>
#include <functional>
#include <optional>

class Enemy final {
private:
	enum class Behavior : uint32_t {
		// 単発攻撃
		OneShot,

		// 十字攻撃(単発攻撃を3回行われたらこれが行われる)
		// また、体力半分以下で回転攻撃に切り替わる
		CrossAttack,

		// 噴火は十字攻撃の際に一定以上プレイヤーが近づいていたら噴火する

		// 噴火(体力半分以上のとき)
		EruptionAttack,
		// 大噴火
		// ・体力が半分以下のとき。体力が50%、20%になった時。
		// ・通常の噴火より攻撃力が高く、飛んでくる弾の数が2倍
		LargeEruptionAttack,

		// ダウン
		Down,
		// チャージ(大噴火時)
		Charge,

		// 何もしない
		None,

		NUM // 数
	};

public:
	Enemy() = default;
	~Enemy() = default;

	Enemy(const Enemy&) = delete;
	Enemy(Enemy&&) = delete;
	Enemy& operator=(const Enemy&) = delete;
	Enemy& operator=(Enemy&&) = delete;

public:
	// 初期化
	void Initialize();

	// 更新
	void Update(const class Player& player, const class Camera& camera);

	// 描画
	void Draw(const class Camera& camera);

	void AfterDraw();

	// 調整
	void Debug(const std::string& guiName);

	bool Collision(const class Player& player);

	void StartAttack();

private:
	void CreateBehaviors();

	void CreateBullets();

	/// <summary>
	/// 次の行動を決める
	/// </summary>
	void SettingBehavior();

	void NextBullet();

public:
	const std::list<std::unique_ptr<Bullet>>& GetBullets() const {
		return bullets_;
	}

	bool IsGameClear() const {
		return hp_ <= 0.0f;
	}

	const Vector3& GetPos() const {
		return model_->pos;
	}

private:
	// モデル
	std::unique_ptr<Model> model_;

	std::unordered_map<std::optional<Behavior>, std::function<void(void)>> behavior_;
	// 各行動時間
	std::unordered_map<std::optional<Behavior>, float> behaviorFinishTime_;
	// 行動開始からの時間
	float behaviorTime_;
	// 今の行動
	std::optional<Behavior> currentBehavior_;
	// 次の行動
	std::optional<Behavior> nextBehavior_;

	// 弾
	std::list<std::unique_ptr<Bullet>> bullets_;
	// 今の弾
	decltype(bullets_)::iterator currentBullet_;

	uint32_t oneShoeCount_;

	// 体力
	float hp_;

	// 半径
	float radius_;

	// プレイヤーへの方向ベクトル
	Vector3 toPlayer_;
	float toPlayerLength_;
	Vector3 playerPos_;

	// 動いてるときのパーティクル
	std::unique_ptr<Particle> particle_;

	std::unique_ptr<Camera> particleCamera_;

	bool isAttackStart_;

	std::unique_ptr<Camera> uiCamera_;
	std::unique_ptr<Texture2D> uiFrame_;
	std::unique_ptr<Texture2D> uiHp_;
};