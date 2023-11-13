#pragma once
#include <string>
#include <format>
#include <concepts>
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector3.h"
#include "Utils/Math/Vector4.h"

#include <Windows.h>
#undef max
#undef min

/// <summary>
/// ログ関連
/// </summary>
namespace Log {
	/// <summary>
	/// 実行ログをファイルに出力する
	/// </summary>
	/// <param name="text">ログテキスト</param>
	/// <returns>成功したか否か</returns>
	bool AddLog(const std::string& text);

	/// <summary>
	/// デバッグ時にVSの出力にデバッグログを出す
	/// </summary>
	/// <param name="text">ログテキスト</param>
	void DebugLog(const std::string& text);

	/// <summary>
	/// デバッグ時にVSの出力にデバッグログを出す
	/// </summary>
	/// <param name="text">ログテキスト</param>
	/// <param name="vec">vector</param>
	void DebugLog(const std::string& text, const Vector2& vec);

	/// <summary>
	/// デバッグ時にVSの出力にデバッグログを出す
	/// </summary>
	/// <param name="text">ログテキスト</param>
	/// <param name="vec">vector</param>
	void DebugLog(const std::string& text, const Vector3& vec);

	/// <summary>
	/// デバッグ時にVSの出力にデバッグログを出す
	/// </summary>
	/// <param name="text">ログテキスト</param>
	/// <param name="vec">vector</param>
	void DebugLog(const std::string& text, const Vector4& vec);

	/// <summary>
	/// デバッグ時にVSの出力にデバッグログを出す
	/// </summary>
	/// <param name="text">ログテキスト</param>
	/// <param name="vec">vector</param>
	template<class T>
	void DebugLog(const std::string& text, const T& number) {
		OutputDebugStringA((text + std::string{ " : " } + std::format("{}", number) + "\n").c_str());
	}

	/// <summary>
	/// 今の時間を文字列として出力する
	/// </summary>
	/// <returns>今の時間(jst)</returns>
	std::string NowTime();
}