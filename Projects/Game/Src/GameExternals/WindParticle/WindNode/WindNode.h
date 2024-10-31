#pragma once
#include "Math/Vector3.h"
#include "Utils/Easeing.h"

class WindNode {
/// <summary>
/// デフォルト定義
/// </summary>
public:
	WindNode() = default;
	WindNode(const WindNode&) = default;
	WindNode(WindNode&&) = default;
	~WindNode() = default;

	WindNode& operator=(const WindNode&) = default;
	WindNode& operator=(WindNode&&) = default;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="start">スタート地点</param>
	/// <param name="speed">1秒間で動く速度</param>
	/// <param name="length">lineの長さ</param>
	void Initialize(
		const Vector3& start, 
		float speed, 
		float length
	);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="direction">方向</param>
	/// <param name="minRange">範囲の最小値</param>
	/// <param name="maxRange">範囲の最大値</param>
	void Update(
		const Vector3& direction, 
		const Vector3& minRange,
		const Vector3& maxRange
	);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const class Camera& camera);

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

private:
	// 初め
	Vector3 start_;
	// 終わり
	Vector3 end_;

	// 速度
	float speed_;
	// 長さ
	float length_;

private:
	// 最初のイージング
	Easeing ease_;

private:
	bool isActive_;
};