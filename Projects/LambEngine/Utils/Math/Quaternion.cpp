#include "Quaternion.h"
#include "Mat4x4.h"
#include <cmath>

const Quaternion Quaternion::identity = { 0.0f, 0.0f, 0.0f, 1.0f };

Quaternion::Quaternion():
	m_{0.0f}
{}

Quaternion::Quaternion(const Quaternion& right):
	Quaternion()
{
	*this = right;
}
Quaternion::Quaternion(Quaternion&& right) noexcept :
	Quaternion()
{
	*this = right;
}

Quaternion::Quaternion(const Vector4& right):
	Quaternion()
{
	*this = right;
}

Quaternion::Quaternion(const Vector3& right, float w) {
	vector_.vector3_ = right;
	vector_.w_ = w;
}

Quaternion::Quaternion(const std::array<float, 4>& right) {
	m_ = right;
}

Quaternion::Quaternion(float x, float y, float z, float w) {
	m_ = { x,y,z,w };
}

Quaternion& Quaternion::operator=(const Quaternion& right) {
	m_ = right.m_;

	return *this;
}
Quaternion& Quaternion::operator=(Quaternion&& right)noexcept {
	m_ = std::move(right.m_);

	return *this;
}

Quaternion& Quaternion::operator=(const Vector4& right) {
	m_ = right.m;

	return *this;
}

Quaternion Quaternion::operator*(const Quaternion& right) const {
	Quaternion result;

	result = Quaternion{
		vector_.vector3_.Cross(right.vector_.vector3_) + vector_.vector3_ * right.vector_.w_ + right.vector_.vector3_ * vector_.w_,
		vector_.w_ * right.vector_.w_ - vector_.vector3_.Dot(right.vector_.vector3_)
	};

	return result;
}
Quaternion& Quaternion::operator*=(const Quaternion& right) {
	*this = *this * right;

	return *this;
}

Quaternion Quaternion::operator*(float right) const {
	Quaternion result{ *this };
	for (auto& i : result.m_) {
		i *= right;
	}
	return result;
}
Quaternion operator*(float right, const Quaternion& left) {
	return left * right;
}
Quaternion& Quaternion::operator*=(float right) {
	*this = *this * right;

	return *this;
}


Quaternion Quaternion::operator/(float right) const {
	return *this * (1.0f / right);
}
Quaternion operator/(float right, const Quaternion& left) {
	return left / right;
}
Quaternion& Quaternion::operator/=(float right) {
	*this = *this / right;

	return *this;
}
/// ========================================================================
/// 等値演算子
/// ========================================================================
#pragma region Equal operator
bool Quaternion::operator==(const Quaternion& right) const {
	return m_ == right.m_;
}
bool Quaternion::operator!=(const Quaternion& right) const {
	return m_ != right.m_;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

Quaternion Quaternion::Conjugate() const {
	return Quaternion{ -vector_.vector3_, vector_.w_ };
}

float Quaternion::Length() const {
	return vector4_.Length();
}

Quaternion Quaternion::Normalize() const {
	return vector4_.Normalize();
}

Quaternion Quaternion::Inverce() const {
	return Conjugate().vector4_ / std::pow(Length(), 2.0f);
}

Mat4x4 Quaternion::GetMatrix() const {
	Mat4x4 result = Mat4x4{
		std::array<Vector4, 4>{
			Vector4{
				std::pow(quaternion_.w_, 2.0f) + std::pow(quaternion_.x_, 2.0f) - std::pow(quaternion_.y_, 2.0f) - std::pow(quaternion_.z_, 2.0f),
				2.0f * (quaternion_.x_ * quaternion_.y_ + quaternion_.w_ * quaternion_.z_),
				2.0f * (quaternion_.x_ * quaternion_.z_ - quaternion_.w_ * quaternion_.y_),
				0.0f
			},
			Vector4{
				2.0f * (quaternion_.x_ * quaternion_.y_ - quaternion_.w_ * quaternion_.z_),
				std::pow(quaternion_.w_, 2.0f) - std::pow(quaternion_.x_, 2.0f) + std::pow(quaternion_.y_, 2.0f) - std::pow(quaternion_.z_, 2.0f),
				2.0f * (quaternion_.y_ * quaternion_.z_ + quaternion_.w_ * quaternion_.x_),
				0.0f
			},
			Vector4{
				2.0f * (quaternion_.x_ * quaternion_.z_ + quaternion_.w_ * quaternion_.y_),
				2.0f * (quaternion_.y_ * quaternion_.z_ - quaternion_.w_ * quaternion_.x_),
				std::pow(quaternion_.w_, 2.0f) - std::pow(quaternion_.x_, 2.0f) - std::pow(quaternion_.y_, 2.0f) + std::pow(quaternion_.z_, 2.0f),
				0.0f
			},
			Vector4::wIdy
		}
	};


	return result;
}

Quaternion Quaternion::MakeRotateAxisAngle(const Vector3& axis, float angle) {
	Quaternion result;
	result.vector_.w_ = std::cos(angle * 0.5f);
	result.vector_.vector3_ = axis.Normalize() * std::sin(angle * 0.5f);

	return result;
}