#pragma once
#include "../Object.h"
#include "Transform/Transform.h"

#include <unordered_set>


class TransformComp : public IComp {
public:
	TransformComp(Object* const object);

	~TransformComp();

	void Init() override;

	void UpdateMatrix();

	void SetParent(Lamb::SafePtr<TransformComp>& parent);

	const Mat4x4& GetMatrix() const {
		return worldMatrix_;
	}

	bool IsRootTransForm() const {
		return parent_.empty();
	}

	bool HaveChildren() const {
		return not children_.empty();
	}

	bool HaveParent() const {
		return parent_.have();
	}

public:
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;

private:
	Mat4x4 worldMatrix_;
	Lamb::SafePtr<TransformComp> parent_;
	std::unordered_set<Lamb::SafePtr<TransformComp>> children_;
};