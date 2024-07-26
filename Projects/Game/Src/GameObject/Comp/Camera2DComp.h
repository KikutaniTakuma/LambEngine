#pragma once

#include "CameraComp.h"


class Camera2DComp : public CameraComp {
public:
	using CameraComp::CameraComp;

	~Camera2DComp() = default;

	void Init();

	void LastUpdate();

public:
	const Mat4x4& GetToNdcMatrix() const;

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
	float32_t width_ = 0.0f;
	float32_t height_ = 0.0f;
	float32_t farClip_ = 0.0f;
	float32_t nearClip_ = 0.0f;

	Mat4x4 cameraMatrix_ = Mat4x4::kIdentity;
	Mat4x4 othographicMatrix_ = Mat4x4::kIdentity;
};