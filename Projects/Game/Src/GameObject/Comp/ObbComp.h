#pragma once

#include "../Object.h"
#include "TransformComp.h"
#include "Utils/Flg.h"

#include <memory>


class ObbComp : public IComp {
public:
	using IComp::IComp;

	~ObbComp() = default;

	void Init() override;

	/// <summary>
	/// 移動したら呼び出すこと
	/// </summary>
	void UpdatePosAndOrient();

	void Draw() override;

	[[nodiscard]] bool IsCollision(Vector3 pos, float radius);
	[[nodiscard]] bool IsCollision(ObbComp* const other);
	[[nodiscard]] bool IsCollision(ObbComp* const other, Vector3& pushVector);

public:
	Vector3 scale = Vector3::kIdentity;
	Vector3 center;


private:
	Lamb::SafePtr<TransformComp> transformComp_;

#ifdef _DEBUG
private:
	uint32_t color_;
#endif // _DEBUG

private:
	static std::unique_ptr<std::array<const Vector3, 8>> localPositions_;
	static std::unique_ptr<std::array<const Vector3, 3>> localOrientations_;
private:
	std::unique_ptr<std::array<Vector3, 8>> positions_;
	std::unique_ptr<std::array<Vector3, 3>> orientations_;

	Lamb::Flg isCollision_;

	/// <summary>
	/// ゲッター
	/// </summary>
public:
	const Lamb::Flg& GetIsCollision() const {
		return isCollision_;
	}
};