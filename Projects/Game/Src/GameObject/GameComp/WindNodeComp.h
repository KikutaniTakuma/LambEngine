#pragma once
#include "../Object.h"
#include "Utils/Easeing.h"

class WindNodeComp : public IComp {
public:
	using IComp::IComp;

	~WindNodeComp() = default;

public:
	void Start();

	void Move()override;

	void Draw() override;

private:
	/// <summary>
	/// 範囲内に入っているか
	/// </summary>
	/// <param name="minRange">範囲の最小値</param>
	/// <param name="maxRange">範囲の最大値</param>
	/// <returns>入っていたらtrue</returns>
	bool IsInRange(
		const Vector3& pos,
		const Vector3& minRange,
		const Vector3& maxRange
	);

public:
	/// <summary>
	/// アクティブか
	/// </summary>
	/// <returns>アクティブならtrue</returns>
	inline bool GetIsActive() const {
		return isActive_;
	}

public:
	// 初め
	Vector3 start;
	// 終わり
	Vector3 end;

	// 速度
	float32_t speed;
	// 長さ
	float32_t length;

	// 方向
	Vector3 direction;

	Vector3 minRange;
	Vector3 maxRange;


private:
	// 最初のイージング
	Easeing ease_;

private:
	bool isActive_;
};
