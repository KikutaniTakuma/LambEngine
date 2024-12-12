#pragma once
#include <chrono>
#include "Math/Vector2.h"
#include <cstdint>

namespace Lamb {
	/// <summary>
	/// デルタタイム
	/// </summary>
	/// <returns>デルタタイム</returns>
	float DeltaTime();

	/// <summary>
	/// クライアントサイズ
	/// </summary>
	/// <returns></returns>
	Vector2 ClientSize();

	/// <summary>
	/// ウィンドウサイズ
	/// </summary>
	/// <returns></returns>
	Vector2 WindowSize();

	/// <summary>
	/// 現在のフレーム時間
	/// </summary>
	/// <returns></returns>
	std::chrono::steady_clock::time_point ThisFrameTime();

	/// <summary>
	/// 終了したか
	/// </summary>
	/// <returns></returns>
	bool IsEngineFianlize();


	/// <summary>
	/// メッシュシェーダーを使えるか
	/// </summary>
	/// <returns></returns>
	bool IsCanUseMeshShader();

	/// <summary>
	/// 今のGPUリソースのインデックス
	/// </summary>
	/// <returns></returns>
	uint32_t GetGraphicBufferIndex();
}