#pragma once

#include "../Object.h"


class Camera3DComp : public IComp {
public:
	using IComp::IComp;

	~Camera3DComp() = default;

	void Init() override;

	void Update() override;

public:
	const Mat4x4& GetToNdcMatrix() const;

public:
	void SetFov(float32_t fov) {
		fov_ = fov;
	}

	void SetFarClip(float32_t farClip) {
		farClip_ = farClip;
	}
	void SetNearClip(float32_t nearClip) {
		nearClip_ = nearClip;
	}

	void SetAspectRatio(float32_t aspectRatio) {
		aspectRatio_ = aspectRatio;
	}

private:
	Lamb::SafePtr<class CameraComp> cameraComp_;

	float32_t fov_ = 0.0f;
	float32_t farClip_ = 0.0f;
	float32_t nearClip_ = 0.0f;
	float32_t aspectRatio_ = 0.0f;

	Mat4x4 projectionMatrix_ = Mat4x4::kIdentity;

};