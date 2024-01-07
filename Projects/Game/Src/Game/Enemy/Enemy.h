#pragma once
#include "Drawers/Model/Model.h"
#include "../Bullet/Bullet.h"
#include <memory>
#include <array>
#include <unordered_map>
#include <functional>
#include <optional>

class Enemy final {
private:
	enum class Behavior : uint32_t {
		// 回転攻撃
		SpinningAttack,
		// 噴火
		EruptionAttack,
		// 大噴火
		LargeEruptionAttack,
		// 十字攻撃
		CrossAttack,

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
	void Update();

	// 描画
	void Draw(const class Camera& camera);

	// 調整
	void Debug(const std::string& guiName);

	void Collision(const class Player& player);

private:
	void CreateBehaviors();

	void CreateBullets();

public:
	const std::list<std::unique_ptr<class Bullet>>& GetBullets() const {
		return bullets_;
	}

	bool IsGameClear() const {
		return hp_ < 0.0f;
	}

private:
	// モデル
	std::unique_ptr<Model> model_;

	std::unordered_map<std::optional<Behavior>, std::function<void(void)>> behavior_;

	std::list<std::unique_ptr<class Bullet>> bullets_;

	// 体力
	float hp_;
};