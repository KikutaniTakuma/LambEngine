#pragma once
#include <memory>
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Other/WaterTex2D/WaterTex2D.h"

class Water final {
private:
	Water() = default;

public:
	~Water() = default;

public:
	static void Initialize();

	static void Finalize();

	static Water* const GetInstance();

private:
	static Water* instance_;


public:
	void Init();

	void Update(const Vector3& cameraPos);

	void Draw(const Mat4x4& cameraMat, PeraRender* const pera = nullptr);

	void Debug(const std::string& guiName);

	void SetWaveData(const WaterTex2D::WaveData& waveData);

public:
	Transform transform;

private:
	std::unique_ptr<PeraRender> luminate_;
	std::unique_ptr<PeraRender> gaussianBlurWidth_;
	std::unique_ptr<PeraRender> gaussianBlurHeight_;
	std::unique_ptr<WaterTex2D> waterSurface_;
	uint32_t color_ = 0u;
	Vector2 randomVec_;
	float32_t density_ = 1.0f;
	int32_t edgeDivision_ = 64;
	int32_t insideDivision_ = 64;
	WaterTex2D::WaveData waveData_;
	Light light_;
	float32_t lightScale_ = 0.0f;
	Vector3 lightRotate_;

	Lamb::SafePtr<class GaussianBlur> gaussianBlurObjectWidth_;
	Lamb::SafePtr<class GaussianBlur> gaussianBlurObjectHeight_;
};