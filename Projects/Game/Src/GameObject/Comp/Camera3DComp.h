#pragma once

#include "CameraComp.h"


class Camera3DComp : public CameraComp {
public:
	using CameraComp::CameraComp;

	~Camera3DComp() = default;

	void Init();

	void LastUpdate();

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

	const Vector3& GetPos() const;

	void Debug(const std::string& guiNaneme);

private:
	float32_t fov_ = 0.0f;
	float32_t farClip_ = 0.0f;
	float32_t nearClip_ = 0.0f;
	float32_t aspectRatio_ = 0.0f;

	Mat4x4 projectionMatrix_ = Mat4x4::kIdentity;

#ifdef _DEBUG
	float32_t scaleSpeed_ = 0.001f;
	float32_t rotateSpeed_ = 0.3f;
	float32_t translateSpeed_ = 1.5f;

	bool isRotateInverse_ = true;
	bool isTranslateInverse_ = false;

	bool isDebug_ = true;

	Vector3 eulerRotate_;

#endif // _DEBUG

};