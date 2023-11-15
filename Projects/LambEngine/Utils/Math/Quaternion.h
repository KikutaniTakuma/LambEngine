#pragma once
#include "Vector3.h"
#include "Vector4.h"

/// <summary>
/// クォータニオンクラス
/// </summary>
class Quaternion {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	Quaternion();
	Quaternion(const Quaternion& right);
	Quaternion(Quaternion&& right) noexcept;
	Quaternion(const Vector4& right);
	Quaternion(const Vector3& right, float w);
	Quaternion(const std::array<float, 4>& right);
	Quaternion(float x, float y, float z, float w);
	~Quaternion() = default;

/// <summary>
/// 演算子のオーバーロード
/// </summary>
public:
	Quaternion& operator=(const Quaternion& right);
	Quaternion& operator=(Quaternion&& right)noexcept;

	Quaternion& operator=(const Vector4& right);

	Quaternion operator*(const Quaternion& right) const;
	Quaternion& operator*=(const Quaternion& right);

public:
	/// <summary>
	/// 共役
	/// </summary>
	/// <returns>このクォータニオンの虚部を反転させたもの</returns>
	Quaternion Conjugate() const;

	/// <summary>
	/// 長さ取得
	/// </summary>
	/// <returns>ノルム</returns>
	float Length() const;

	/// <summary>
	/// 長さ1のクォータニオンを返す
	/// </summary>
	/// <returns>単位クォータニオン</returns>
	Quaternion Normalize() const;

	/// <summary>
	/// 逆クォータニオンを返す
	/// </summary>
	/// <returns>逆クォータニオン</returns>
	Quaternion Inverce() const;

	class Mat4x4 GetMatrix() const;

/// <summary>
/// 静的メンバ関数
/// </summary>
public:
	static Quaternion MakeRotateAxisAngle(const Vector3& axis, float angle);


/// <summary>
/// 静的メンバ変数
/// </summary>
public:
	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 0.0f w = 1.0f
	/// </summary>
	static const Quaternion identity;

/// <summary>
/// メンバ変数
/// </summary>
public:
	union
	{
		std::array<float, 4> m_;
		__m128 m128_;
		Vector4 vector4_;
		struct {
			Vector3 vector3_;
			float w_;
		}vector_;

		struct {
			float x_;
			float y_;
			float z_;
			float w_;
		}quaternion_;
	};
};