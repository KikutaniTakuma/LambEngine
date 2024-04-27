#pragma once
#include "Drawers/Model/Model.h"
#include "Utils/SafePtr/SafePtr.h"

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

private:
	// ブロックのモデル
	Lamb::SafePtr<Model> model_;

	// 浮いてるか
	bool isFloating_;

	Transform transform_;
};