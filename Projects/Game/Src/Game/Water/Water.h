#pragma once
#include <memory>
#include "Drawer/PeraRender/PeraRender.h"
#include "Drawer/Other/WaterTex2D/WaterTex2D.h"


/// <summary>
/// 水
/// </summary>
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
	static Water* pInstance_;


public:
	void Init();

	void Update();

	void Draw(const Mat4x4& cameraMat, PeraRender* const pera = nullptr);

	void Debug(const std::string& guiName);

	void SetWaveData(const WaterTex2D::WaveData& waveData);

	const Vector2& GetRandomVector() const {
		return randomVec_;
	}
	float32_t GetDensity() const {
		return density_;
	}

	float CalcWaveHeight(float32_t2 uv);

	void SetCameraPos(const float32_t3& pos);

	void StartWave();

	void StopWave();

private:
	float32_t3 cameraPos_;


public:
	Transform transform;

private:
	std::unique_ptr<WaterTex2D> waterSurface_;
	float32_t4 color_ = 0u;
	float32_t2 randomVec_;

	float32_t waveTime_ = 0.0f;

	float32_t divisionMinLength = 5.0f;
	float32_t density_ = 1.0f;
	int32_t edgeDivision_ = 64;
	int32_t insideDivision_ = 64;
	WaterTex2D::WaveData waveData_;

	bool isActiveWave_ = false;
#ifdef USE_DEBUG_CODE
	bool debugIsActiveWave_ = false;
#endif // USE_DEBUG_CODE

	std::array<bool, WaterTex2D::kMaxRipplePoint> isPoint_ = {false};
	float32_t nextRipplePointLength_ = 0.5f;
	float32_t time_ = 0.0f;
	float32_t nextRipplePoint_ = 0.5f;
	size_t index_ = 0;

	Lamb::SafePtr<class GaussianBlur> gaussianBlurObjectWidth_;
	Lamb::SafePtr<class GaussianBlur> gaussianBlurObjectHeight_;
};