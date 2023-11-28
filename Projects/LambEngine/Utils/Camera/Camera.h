#pragma once
#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"
#include <string>

/// <summary>
/// カメラ
/// </summary>
class Camera {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	Camera() noexcept;
	Camera(const Camera& right) noexcept = default;
	Camera(Camera&& right) noexcept = default;
	~Camera() noexcept = default;


/// <summary>
///  代入演算子
/// </summary>
public:
	Camera& operator=(const Camera& right) noexcept = default;
	Camera& operator=(Camera&& right) noexcept = default;


/// <summary>
/// メンバ関数
/// </summary>
public:
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 行列更新
	/// </summary>
	/// <param name="worldMat">カメラの親のワールド行列</param>
	void Update(const Mat4x4& worldMat);

	void SetMat4x4Update(const Mat4x4& worldMat);

	/// <summary>
	/// ビュー行列取得
	/// </summary>
	/// <returns></returns>
	const Mat4x4& GetView() const {
		return view;
	}

	/// <summary>
	/// プロジェクション行列取得
	/// </summary>
	/// <returns></returns>
	const Mat4x4& GetProjection() const {
		return projection;
	}

	/// <summary>
	/// 平行投影
	/// </summary>
	/// <returns></returns>
	const Mat4x4& GetOthographics() const {
		return othograohics;
	}

	/// <summary>
	/// 透視投影行列を取得
	/// </summary>
	/// <returns>透視投影行列</returns>
	inline const Mat4x4& GetViewProjection() const noexcept {
		return viewProjecction;
	}
	
	/// <summary>
	/// 平衡投影行列を取得
	/// </summary>
	/// <returns></returns>
	inline const Mat4x4& GetViewOthographics() const noexcept {
		return viewOthograohics;
	}

	/// <summary>
	/// カメラのポジションを取得
	/// </summary>
	/// <returns>カメラのポジション</returns>
	const Vector3& GetPos() const {
		return worldPos;
	}

	/// <summary>
	/// 透視投影行列にビューポートをかけたものを取得
	/// </summary>
	/// <returns></returns>
	inline const Mat4x4& GetViewProjectionVp() const noexcept {
		return viewProjecctionVp;
	}
	/// <summary>
	/// 平衡投影行列ビューポートをかけたものを取得
	/// </summary>
	/// <returns></returns>
	inline const Mat4x4& GetViewOthographicsVp() const noexcept {
		return viewOthograohicsVp;
	}

	void Debug(const std::string& guiName);

	Vector3 GetDirection() const {
		return (Vector3::zIdy * MakeMatrixRotate(rotate)).Normalize();
	}

	Vector3 GetDirection(const Vector3& basis, const Vector3& target) const {
		Vector3 posTmp = pos;
		posTmp.y = 0.0f;
		return (Vector3::zIdy * DirectionToDirection(basis.Normalize(), (target - posTmp).Normalize())).Normalize();
	}

	const Mat4x4& GetRotateMatrix() const {
		return rotateMat_;
	}


/// <summary>
/// メンバ変数
/// </summary>
public:
	Vector3 pos;
	Vector3 scale;
	Vector3 rotate;

	float drawScale;

protected:
	Vector3 worldPos;

protected:
	static constexpr float kNearClip = 0.01f;

public:
	float farClip;
	float fov;

protected:
	Mat4x4 view;
	Mat4x4 projection;
	Mat4x4 othograohics;

	Mat4x4 viewProjecction;
	Mat4x4 viewOthograohics;

	Mat4x4 viewProjecctionVp;
	Mat4x4 viewOthograohicsVp;

	Mat4x4 rotateMat_;
};