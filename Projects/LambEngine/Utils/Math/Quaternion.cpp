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