#pragma once
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

	Transform& operator=(const QuaternionTransform& transform);
};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;

	QuaternionTransform& operator=(const Transform& transform);
};

inline QuaternionTransform& QuaternionTransform::operator=(const Transform& transform) {
	this->scale = transform.scale;
	this->rotate = Quaternion::EulerToQuaternion(transform.rotate);
	this->translate = transform.translate;

	return *this;
}

inline Transform& Transform::operator=(const QuaternionTransform& transform) {
	this->scale = transform.scale;
	this->rotate = Vector3::QuaternionToEuler(transform.rotate);
	this->translate = transform.translate;

	return *this;
}