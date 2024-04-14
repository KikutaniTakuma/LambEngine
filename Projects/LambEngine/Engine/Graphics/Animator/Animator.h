#pragma once
#include <string>
#include "Math/Mat4x4.h"
#include "Engine/Graphics/GraphicsStructs.h"

class Animator {
public:
	Animator();
	Animator(const Animator&)= default;
	Animator(Animator&&) = default;
	~Animator() = default;

	Animator& operator=(const Animator&) = default;
	Animator& operator=(Animator&&) = default;

public:
	void Load(const std::string& fileName);

	void Update(const Mesh* const mesh);

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
	[[nodiscard]] const Mat4x4& GetLocalMat4x4() const;

	/// <summary>
	/// アニメーションを全て再生するか
	/// </summary>
	/// <param name="isFullAnimation"></param>
	void SetIsFullAnimation(bool isFullAnimation);

	/// <summary>
	/// <para>再生するアニメーションのインデックス</para>
	/// <para>0～要素数にクランプされる</para>
	/// </summary>
	/// <param name="index"></param>
	void SetAnimationIndex(size_t index);

	/// <summary>
	/// <para>アニメーションをループさせるか</para>
	/// <para>isFullAnimationがfalseの場合、設定されたindexのアニメーションでループする</para>
	/// </summary>
	/// <param name="isLoop"></param>
	void SetLoopAnimation(bool isLoop);

private:
	template<class T>
	T CalaclateValue(const AnimationCurve<T>& animationCurve, float time) {
		if (animationCurve.keyFrames.empty()) {
			throw Lamb::Error::Code<Animator>("keyFrams is empty", __func__);
		}
		if (animationCurve.keyFrames.size() == 1 or time < animationCurve.keyFrames.front().time) {
			return animationCurve.keyFrames.front().value;
		}

		for (size_t index = 0; index < animationCurve.keyFrames.size() - 1; index++) {
			size_t nextIndex = index + 1;
			if (animationCurve.keyFrames[index].time <= time and time <= animationCurve.keyFrames[nextIndex].time) {
				float t = (time - animationCurve.keyFrames[index].time) / (animationCurve.keyFrames[nextIndex].time - animationCurve.keyFrames[index].time);
				return Vector3::Lerp(animationCurve.keyFrames[index].value, animationCurve.keyFrames[nextIndex].value, t);
			}
		}

		return animationCurve.keyFrames.back().value;
	}

private:
	float animationTime_;
	struct Animations* animations_;
	Mat4x4 animationMatrix_;
	size_t currentAnimationIndex_;

	bool isActive_;

	bool isFullAnimation_;
	bool isLoop_;
};