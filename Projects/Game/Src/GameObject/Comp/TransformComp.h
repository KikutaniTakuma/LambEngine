#pragma once
#include "../Object.h"
#include "Transform/Transform.h"

#include <unordered_set>


class TransformComp : public IComp {
public:
	using IComp::IComp;

	~TransformComp() = default;

	void Init() override;

	void Update() override;

	void SetParent(Lamb::SafePtr<TransformComp>& parent);

	const Mat4x4& GetMatrix() const {
		return worldMatrix_;
	}

	bool IsRootTransForm() const {
		return parent_.empty();
	}

	bool HaveChildlen() const {
		return not childlen_.empty();
	}

	bool HaveParent() const {
		return parent_.have();
	}

public:
	Vector3 scale = Vector3::kIdentity;
	Quaternion rotate;
	Vector3 translate;

private:
	Mat4x4 worldMatrix_;
	Lamb::SafePtr<TransformComp> parent_ = nullptr;
	std::unordered_set<Lamb::SafePtr<TransformComp>> childlen_;
};