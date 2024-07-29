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

	void Draw(CameraComp* cameraComp) override;

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

private:
	/// <summary>
	/// 範囲内に入っているか
	/// </summary>
	/// <param name="min">範囲の最小値</param>
	/// <param name="max">範囲の最大値</param>
	/// <returns>入っていたらtrue</returns>
	bool IsInRange(
		const Vector3& pos, const Vector3& min, const Vector3& max
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
