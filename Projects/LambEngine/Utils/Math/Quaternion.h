#pragma once
#include <array>
#include <immintrin.h>
#include "Vector3.h"
#include "Vector4.h"

class Quaternion {
public:
	Quaternion();
	Quaternion(const Quaternion& right);
	Quaternion(Quaternion&& right) noexcept;
	Quaternion(const Vector4& right);
	Quaternion(const Vector3& right, float w);
	Quaternion(const std::array<float, 4>& right);
	Quaternion(float x, float y, float z, float w);
	~Quaternion() = default;

public:
	Quaternion& operator=(const Quaternion& right);
	Quaternion& operator=(Quaternion&& right)noexcept;

	Quaternion& operator=(const Vector4& right);

	Quaternion operator*(const Quaternion& right) const;
	Quaternion& operator*=(const Quaternion& right);

	/// <summary>
	/// 共役
	/// </summary>
	/// <returns>このクォータニオンの虚部を反転させたもの</returns>
	Quaternion Conjugate() const;

	float Length() const;

	Quaternion Normalize() const;

	Quaternion Inverce() const;

public:
	static const Quaternion identity;

public:
	union
	{
		std::array<float, 4> m_;
		__m128 m128;
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