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

	void UpdateChildrenMatrix();
	void UpdateParentMatrix();

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

	void Debug(const std::string& guiName) override;

public:
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
#ifdef _DEBUG
	Vector3 eulerRotate;
#endif // _DEBUG

private:
	Mat4x4 worldMatrix_;
	Lamb::SafePtr<TransformComp> parent_;
	std::unordered_set<Lamb::SafePtr<TransformComp>> children_;
};