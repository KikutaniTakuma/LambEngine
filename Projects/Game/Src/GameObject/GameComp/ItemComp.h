#pragma once
#include "../Object.h"
class ItemComp : public IComp {
public:
	using IComp::IComp;

	virtual ~ItemComp() = default;

public:
	void Init() override;

	void Start();

	Vector3 GetVelocity() const;

protected:
	float32_t velocity_ = 0;

	Lamb::Flg isStart_;

	Lamb::SafePtr<class TransformComp> transform_;
};
