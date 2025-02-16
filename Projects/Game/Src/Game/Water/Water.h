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

	void Update(const Vector3& cameraPos);

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

private:
	float32_t3 cameraPos_;


public:
	Transform transform;

private:
	std::unique_ptr<PeraRender> luminate_;
	std::unique_ptr<PeraRender> gaussianBlurWidth_;
	std::unique_ptr<PeraRender> gaussianBlurHeight_;
	std::unique_ptr<WaterTex2D> waterSurface_;
	Vector4 color_ = 0u;
	Vector2 randomVec_;

	float32_t divisionMinLength = 5.0f;
	float32_t density_ = 1.0f;
	int32_t edgeDivision_ = 64;
	int32_t insideDivision_ = 64;
	WaterTex2D::WaveData waveData_;
	Light light_;
	float32_t lightScale_ = 0.0f;
	Vector3 lightRotate_;

	std::array<bool, WaterTex2D::kMaxRipplePoint> isPoint_ = {false};
	float32_t time_ = 0.0f;
	float32_t nextRipplePoint_ = 0.5f;
	size_t index_ = 0;

	Lamb::SafePtr<class GaussianBlur> gaussianBlurObjectWidth_;
	Lamb::SafePtr<class GaussianBlur> gaussianBlurObjectHeight_;

#ifdef USE_DEBUG_CODE
	bool isWaveAddTime_ = false;
#endif // USE_DEBUG_CODE

};