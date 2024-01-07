#pragma once
#include "Drawers/Model/Model.h"
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

private:
	// モデル
	std::unique_ptr<Model> model_;

	// 最大速度
	const float kMaxSpeed_ = 10.0f;

	// 速度加算
	float addSpeed_;

	// 速度
	float speed_;

	// 速度減衰
	float speedDecay_;

	// 移動ベクトル
	Vector3 moveVector_;

	// 真ん中からのoffset
	Vector3 offset_;


	// 攻撃値
	float attack_;
};