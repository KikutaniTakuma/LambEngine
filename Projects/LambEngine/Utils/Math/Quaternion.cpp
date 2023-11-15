#include "Quaternion.h"
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
	vec_.vec3_ = right;
	vec_.w_ = w;
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
		vec_.vec3_.Cross(right.vec_.vec3_) + vec_.vec3_ * right.vec_.w_ + right.vec_.vec3_ * vec_.w_,
		vec_.w_ * right.vec_.w_ - vec_.vec3_.Dot(right.vec_.vec3_)
	};

	return result;
}
Quaternion& Quaternion::operator*=(const Quaternion& right) {
	*this = *this * right;

	return *this;
}

Quaternion Quaternion::Conjugate() const {
	return Quaternion{ -vec_.vec3_, vec_.w_ };
}

float Quaternion::Length() const {
	return vec4_.Length();
}

Quaternion Quaternion::Normalize() const {
	return vec4_.Normalize();
}

Quaternion Quaternion::Inverce() const {
	return Conjugate().vec4_ / std::pow(Length(), 2.0f);
}