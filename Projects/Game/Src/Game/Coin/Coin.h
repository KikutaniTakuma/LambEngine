#pragma once
#include "Drawers/AnimationModel/AnimationModel.h"
#include "Utils/SafePtr.h"
#include "Utils/Flg.h"
#include "Game/CollisionManager/Obb/Obb.h"
#include <memory>

class Coin {
public:
	Coin() = default;
	Coin(const Coin&) = default;
	Coin(Coin&&) = default;
	~Coin() = default;

	Coin& operator=(const Coin&) = default;
	Coin& operator=(Coin&&) = default;

public:
	void Init(const Transform& transform);

	void Update();

	void Draw(const class Camera& camera);

	const Lamb::Flg& GetIsPunch() const {
		return isPunch_;
	}

public:
	const Vector3& GetTranslate() const {
		return transform_.translate;
	}
	const Vector3& GetScale() const {
		return transform_.scale;
	}

	void SetIsCollision(bool isCollision) {
		if (isCollision_) {
			return;
		}
		isCollision_ = isCollision;
	}

	Obb& GetObb() {
		if (not obb_) {
			throw Lamb::Error::Code<Obb>("obb is nullptr(must call Init)", ErrorPlace);
		}
		return *obb_;
	}

private:
	ObbPtr obb_;

	// ブロックのモデル
	Lamb::SafePtr<Model> model_;

	// アニメーション
	std::unique_ptr<Animator> animator_;

	Transform transform_;

	// パンチしたか
	Lamb::Flg isPunch_;

	Lamb::Flg isCollision_;
};