#pragma once
#include "Math/Vector3.h"
#include "WindNode/WindNode.h"
#include <utility>
#include <list>
#include <memory>

class WindParticle {
public:
	WindParticle() = default;
	WindParticle(const WindParticle&) = delete;
	WindParticle(WindParticle&&) = delete;
	~WindParticle() = default;

	WindParticle& operator=(const WindParticle&) = delete;
	WindParticle& operator=(WindParticle&&) = delete;

public:
	/// <summary>
	/// 初期化関数
	/// </summary>
	void Initialize(
		const Vector3& pos,
		const Vector3& size,
		const Vector3& direction
	);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const class Camera& camera);

private:
	/// <summary>
	/// 風を消す処理
	/// </summary>
	void DeleteParticle();

	/// <summary>
	/// 風を追加する
	/// </summary>
	void CreatParticle();

private:
	// これを中心とした矩形に風が描画
	Vector3 pos_;

	// この大きさで風の描画がなされる
	Vector3 size_;

	// 風の向き(デフォルトはZ方向)
	Vector3 direction_;

	
private:
	// 生成場所ランダム範囲
	std::pair<Vector3, Vector3> positionRange_;

	// 長さランダム範囲
	std::pair<float, float> lengthRange_;

	// 速度ランダム範囲
	std::pair<float, float> speedRange_;

	//一度に出る個数のランダム範囲
	std::pair<uint32_t, uint32_t> appearNumberRange_;

	// 出現頻度のランダム範囲
	std::pair<uint32_t, uint32_t> appearFrequencyRange_;
	// 出現頻度のランダム範囲がこの値以下なら生成する
	uint32_t appearNum_;

private:
	// パーティクルの情報を保存する
	std::list<std::unique_ptr<WindNode>> particle_;
};