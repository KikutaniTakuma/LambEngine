#pragma once
#include "Drawers/Model/Model.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Utils/Flg/Flg.h"
#include "Game/CollisionManager/Obb/Obb.h"

class SkyBlock {
public:
	SkyBlock() = default;
	SkyBlock(const SkyBlock&) = default;
	SkyBlock(SkyBlock&&) = default;
	~SkyBlock() = default;

	SkyBlock& operator=(const SkyBlock&) = default;
	SkyBlock& operator=(SkyBlock&&) = default;

public:
	void Init(const Transform& transform);

	void Update();

	void SetIsCollisionOtherBlock(bool isCollision);

	void AfterCollisionUpdate(const Vector3& pushVector);

	void Draw(const class Camera& camera);

	void StartFall();

	void Debug(const std::string& guiName);

	Obb& GetObb() {
		if (not obb_) {
			throw Lamb::Error::Code<Obb>("obb is nullptr(must call Init)", __func__);
		}
		return *obb_;
	}

private:
	void Falling();
	void StopFalling();

private:
	ObbPtr obb_;

	// ブロックのモデル
	Lamb::SafePtr<Model> model_;

	// 落ちてるか
	Lamb::Flg isFall_;
	// 他のブロックに当たっているか
	Lamb::Flg isCollisionBlock_;

	Transform transform_;

	float gravity_ = 0.0f;
	float speed_ = 0.0f;
	float fallingTime_ = 0.0f;
};