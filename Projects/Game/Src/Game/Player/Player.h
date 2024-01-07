#pragma once
#include "Drawers/Model/Model.h"
#include <memory>
#include <numbers>

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

public:
	inline const Vector3& GetPos() const {
		return model_->pos;
	}

	inline float GetRotate() const {
		return rotate_;
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
};