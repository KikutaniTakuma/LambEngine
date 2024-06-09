#pragma once
#include "../Object.h"
#include "Transform/Transform.h"

#include <memory>


class TransformComp : public IComp {
public:
	using IComp::IComp;

	~TransformComp() = default;

	void Init() override;

	void Update() override;

	void SetParent(TransformComp* parent);

	const Mat4x4& GetMatrix() const {
		return worldMatrix_;
	}

public:
	Vector3 scale = Vector3::kIdentity;
	Quaternion rotate;
	Vector3 translate;

private:
	Mat4x4 worldMatrix_;
	Lamb::SafePtr<TransformComp> parent_ = nullptr;
};