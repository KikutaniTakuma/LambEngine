#pragma once

#include "../Object.h"


class Camera2DComp : public IComp {
public:
	using IComp::IComp;

	~Camera2DComp() = default;

	void Init();

	void Update();

	void Save(nlohmann::json& json) override;
	void Load(nlohmann::json& json) override;

public:
	const Mat4x4& GetToNdcMatrix() const;

public:
	void SetWidth(float32_t width) {
		width_ = width;
	}
	void SetHeight(float32_t height) {
		height_ = height;
	}

	void SetNearClip(float32_t nearClip) {
		nearClip_ = nearClip;
	}
	void SetFarClip(float32_t farClip) {
		farClip_ = farClip;
	}

private:
	Lamb::SafePtr<class CameraComp> cameraComp_;

	float32_t width_ = 0.0f;
	float32_t height_ = 0.0f;
	float32_t nearClip_ = 0.0f;
	float32_t farClip_ = 0.0f;

	Mat4x4 othographicMatrix_ = Mat4x4::kIdentity;
};