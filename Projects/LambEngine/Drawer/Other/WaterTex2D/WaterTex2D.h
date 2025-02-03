/// ===========================
/// ==  Water2Dクラスの宣言  ==
/// ===========================

#pragma once

#pragma once
#include "../../BaseDrawer.h"
#include "Engine/Graphics/RenderContextManager/RenderContext/RenderContext.h"
#include "Drawer/AirSkyBox/AirSkyBox.h"

/// <summary>
/// 水面板ポリ描画
/// </summary>
class WaterTex2D : public BaseDrawer {
public:
	static constexpr uint32_t kMaxDrawCount = 1u;

	struct WaveData {
		float32_t waveStrength = 5.0f;
		float32_t ripples = 1.0f;
		float32_t waveSpeed = 0.1f;
		float32_t time = 0.0f;
		float32_t lengthAttenuation = 0.1f;
		float32_t3 ripplesPoint;

		float32_t3 preRipplesPoint;
		float32_t ripplePointSpeed = 0.0f;
	};

	struct ShaderData {
		Vector2 randomVec;
		float32_t density = 0.0_f32;
		uint32_t edgeDivision = 1;
		uint32_t insideDivision = 1;
		float32_t3 camerPosition;
		float32_t distanceThreshold = 1.0_f32;
		float32_t3 cameraDirection;
		float32_t4x4 viewportMatrix;
		WaveData waveData;
		AirSkyBox::AtmosphericParams atomosphericParam;
	};

	using WaterRenderContext = RenderContext<ShaderData, kMaxDrawCount>;
private:

	static const LoadFileNames kFileNames_;

public:
	WaterTex2D();
	WaterTex2D(const WaterTex2D&) = default;
	WaterTex2D(WaterTex2D&&) = default;
	~WaterTex2D() = default;

	WaterTex2D& operator=(const WaterTex2D&) = default;
	WaterTex2D& operator=(WaterTex2D&&) = default;

public:
	/// <summary>
	/// ロード
	/// </summary>
	void Load();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldMatrix">カメラ行列</param>
	/// <param name="camera">カメラ行列</param>
	/// <param name="randomVec">ランダムベクトル</param>
	/// <param name="density">パーリンノイズの値</param>
	/// <param name="divisionMaxLength">分割が最大数になる距離</param>
	/// <param name="edgeDivision">分割数(辺)</param>
	/// <param name="insideDivision">分割数(ポリゴン)</param>
	/// <param name="waveData">(波のデータ)</param>
	/// <param name="color">色</param>
	/// <param name="blend">ブレンド</param>
	void Draw(
		const Mat4x4& worldMatrix,
		const Mat4x4& camera,
		Vector2 randomVec,
		float32_t density,
		float32_t3 cameraPos,
		float32_t divisionMinLength,
		uint32_t edgeDivision,
		uint32_t insideDivision,
		WaveData waveData,
		uint32_t color,
		BlendType blend
	);
};