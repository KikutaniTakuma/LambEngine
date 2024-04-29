#pragma once
#include "Drawers/Model/Model.h"
#include "Utils/SafePtr/SafePtr.h"
#include "Utils/Flg/Flg.h"

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

	void Draw(const class Camera& camera);

	void StartFall();

	void Debug(const std::string& guiName);

private:
	void Falling();
	void StopFalling();

private:
	// ブロックのモデル
	Lamb::SafePtr<Model> model_;

	// 落ちてるか
	Lamb::Flg isFall_;

	Transform transform_;

	float gravity_;
	float speed_;
	float fallingTime_;
};