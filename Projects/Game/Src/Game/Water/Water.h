#pragma once
#include <memory>
#include "Drawer/PeraRender/PeraRender.h"
#include "Drawer/Other/WaterTex2D/WaterTex2D.h"

class Water final {
public:
	enum class Version {
		kFirst,        // 何もしていない初期の水
		kTransparency, // 透過させた
		kDistortion,   // 水面下のオブジェクトを歪ませる
		kCaustics,     // 水面下にコーティクス

		kNum
	};

#ifdef USE_DEBUG_CODE
private:
	static const std::array<std::string, static_cast<size_t>(Version::kNum)> kComboVersionString_;
#endif // USE_DEBUG_CODE

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

	void Update();

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
	Vector4 color_ = 0u;
	WaterTex2D::ShaderData shaderData_;
	WaterTex2D::WaveData waveData_;

	Version currentVersion = Version::kFirst;
};