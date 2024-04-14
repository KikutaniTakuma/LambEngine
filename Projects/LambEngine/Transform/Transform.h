#pragma once
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Mat4x4.h"

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

	Transform& operator=(const Transform&) = default;
	Transform& operator=(Transform&&) = default;

	Transform& operator=(const struct QuaternionTransform& transform);
	Mat4x4 GetMatrix() const;
	void Debug(const std::string& guiName);
};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;

	QuaternionTransform& operator=(const Transform& transform);
	QuaternionTransform& operator=(const QuaternionTransform&) = default;
	QuaternionTransform& operator=(QuaternionTransform&&) = default;

	Mat4x4 GetMatrix() const;
};