#pragma once

#include "../Object.h"
#include "Transform/Transform.h"

#include <memory>


class ObbComp : public IComp {
public:
	using IComp::IComp;

	~ObbComp() = default;

	void Init() override;

	void LastUpdate() override;

	void SetParent(ObbComp* parent);

	bool IsCollision() const;

public:
	Vector3 scale = Vector3::kIdentity;
	Quaternion rotate;
	Vector3 translate;

private:
	Mat4x4 worldMatrix_;
	ObbComp* parent_ = nullptr;
};