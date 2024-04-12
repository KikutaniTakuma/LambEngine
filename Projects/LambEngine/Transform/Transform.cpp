#include "Transform.h"


QuaternionTransform& QuaternionTransform::operator=(const Transform& transform) {
	this->scale = transform.scale;
	this->rotate = Quaternion::EulerToQuaternion(transform.rotate);
	this->translate = transform.translate;

	return *this;
}

Transform& Transform::operator=(const QuaternionTransform& transform) {
	this->scale = transform.scale;
	this->rotate = Vector3::QuaternionToEuler(transform.rotate);
	this->translate = transform.translate;

	return *this;
}

Mat4x4 Transform::GetMatrix() const
{
	return Mat4x4::MakeAffin(scale, rotate, translate);
}

Mat4x4 QuaternionTransform::GetMatrix() const
{
	return Mat4x4::MakeAffin(scale, rotate, translate);
}
