#pragma once
#include "Drawers/Model/Model.h"
#include "Engine/Graphics/Animator/Animator.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Utils/Flg/Flg.h"

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

	void Draw(const class Camera& camera);

public:
	// パンチフラグをtrueにしてモーションを再生する
	void Punch();

private:
	// ブロックのモデル
	Lamb::SafePtr<Model> model_;

	// アニメーション
	Lamb::SafePtr<Animator> animator_;

	// パンチしたか
	Lamb::Flg isPunch_;


};