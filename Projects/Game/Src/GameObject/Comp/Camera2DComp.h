#pragma once

#pragma once

#include "../Object.h"

#include "TransformComp.h"


class Camera2DComp : public IComp {
public:
	using IComp::IComp;

	~Camera2DComp() = default;

	void Init() override;

	void LastUpdate() override;

public:
	const Mat4x4& GetMatrix() const {
		return cameraMatrix_;
	}

public:
	TransformComp& GetTransformComp() {
		return *transform_;
	}
	const TransformComp& GetTransformComp() const {
		return *transform_;
	}
public:
	void SetWidth(float32_t width) {
		width_ = width;
	}
	void SetHeight(float32_t height) {
		height_ = height;
	}

	void SetFarClip(float32_t farClip) {
		farClip_ = farClip;
	}
	void SetNearClip(float32_t nearClip) {
		nearClip_ = nearClip;
	}

private:
	Lamb::SafePtr<TransformComp> transform_;
	float32_t width_ = 0.0f;
	float32_t height_ = 0.0f;
	float32_t farClip_ = 0.0f;
	float32_t nearClip_ = 0.0f;

	Mat4x4 cameraMatrix_ = Mat4x4::kIdentity;
};