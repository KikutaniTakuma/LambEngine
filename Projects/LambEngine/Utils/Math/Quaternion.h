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
/// 単項演算子
/// </summary>
public:
	Quaternion operator+() const;
	Quaternion operator-() const;

/// <summary>
/// 二項演算子のオーバーロード
/// </summary>
public:
	Quaternion& operator=(const Quaternion& right);
	Quaternion& operator=(Quaternion&& right)noexcept;

	Quaternion& operator=(const Vector4& right);

	Quaternion operator*(const Quaternion& right) const;
	Quaternion& operator*=(const Quaternion& right);

	Quaternion operator+(const Quaternion& right) const;
	Quaternion& operator+=(const Quaternion& right);

	Quaternion operator-(const Quaternion& right) const;
	Quaternion& operator-=(const Quaternion& right);

	Quaternion operator*(float right) const;
	friend Quaternion operator*(float right, const Quaternion& left);
	Quaternion& operator*=(float right);

	Quaternion operator/(float right) const;
	friend Quaternion operator/(float right, const Quaternion& left);
	Quaternion& operator/=(float right);

	bool operator==(const Quaternion& right) const;
	bool operator!=(const Quaternion& right) const;

public:
	/// <summary>
	/// 共役
	/// </summary>
	/// <returns>このクォータニオンの虚部を反転させたもの</returns>
	Quaternion Conjugate() const;

	/// <summary>
	/// クォータニオンを4次元ベクトルとしての内積
	/// </summary>
	/// <param name="other">他のクォータニオン</param>
	/// <returns>内積結果</returns>
	float Dot(const Quaternion& other) const;

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

	/// <summary>
	/// 回転行列を取得
	/// </summary>
	/// <returns>クォータニオンからの回転行列</returns>
	class Mat4x4 GetMatrix() const;

/// <summary>
/// 静的メンバ関数
/// </summary>
public:
	static Quaternion MakeRotateAxisAngle(const Vector3& axis, float angle);


/// <summary>
/// 静的メンバ定数
/// </summary>
public:
	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 0.0f w = 1.0f
	/// </summary>
	static const Quaternion identity;

	/// <summary>
	/// x = 0.0f, y = 0.0f, z = 0.0f w = 0.0f
	/// </summary>
	static const Quaternion zero;

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