#pragma once
#include "../Object.h"
class SpriteAnimatorComp : public IComp {
public:
	using IComp::IComp;

	~SpriteAnimatorComp() = default;

public:
	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

public:
	void Init() override;

	void Update() override;

	void LastUpdate() override;

	void Debug(const std::string& guiName);

public:
	// 最初からスタート
	void Start();

	// 止めてリセット
	void Stop();

	// リセット
	void Reset();

	// スタート
	void Restart();

	// 一時停止
	void Pause();

public:
	/// <summary>
	/// アニメーションのマトリックスを取得
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] const Mat4x4& GetUvMat4x4() const {
		return animationMatrix_;
	}

	/// <summary>
	/// アニメーションの枚数
	/// </summary>
	/// <param name="animationNumber"></param>
	void SetAnimationNumber(uint32_t animationNumber);

	/// <summary>
	/// アニメーションの開始位置
	/// </summary>
	void SetStartPos(Vector2 startPos) {
		startPos_ = startPos;
	}

	/// <summary>
	/// <para>アニメーションをループさせるか</para>
	/// <para>isFullAnimationがfalseの場合、設定されたindexのアニメーションでループする</para>
	/// </summary>
	/// <param name="isLoop"></param>
	void SetLoopAnimation(bool isLoop) {
		isLoop_ = isLoop;
	}

	void SetDuration(float duration) {
		duration_ = duration;
	}

	// 向き反転
	void DirectionInverse(bool isSigned);

	//アニメーション中かどうか取得
	bool GetIsActive() const { return isActive_; }

	//現在のアニメーションナンバー取得
	int32_t GetCurrentAnimationNumber() const { return currentAnimationNumber_; }

private:
	Vector3 startPos_;
	Vector3 currentPos_;
	Vector3 scale_ = Vector3::kIdentity;
	int32_t currentAnimationNumber_ = 0u;
	int32_t animationNumber_ = 0u;

	float animationTime_ = 0.0f;
	float duration_ = 0.0f;
	Mat4x4 animationMatrix_;

	bool isActive_ = false;
	bool isLoop_ = false;
	bool isDirectionInverse_ = false;

	Lamb::SafePtr<class SpriteRenderDataComp> spriteRenderDataComp_;
};
