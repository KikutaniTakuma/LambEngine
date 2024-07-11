#pragma once

#pragma once

#include "../Object.h"

#include "TransformComp.h"


class Camera3DComp : public IComp {
public:
	using IComp::IComp;

	~Camera3DComp() = default;

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
	Lamb::SafePtr<TransformComp> transform_;
	float32_t fov_ = 0.0f;
	float32_t farClip_ = 0.0f;
	float32_t nearClip_ = 0.0f;
	float32_t aspectRatio_ = 0.0f;

	Mat4x4 cameraMatrix_ = Mat4x4::kIdentity;
};