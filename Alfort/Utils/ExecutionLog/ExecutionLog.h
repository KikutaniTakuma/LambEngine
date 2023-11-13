#pragma once
#include <string>

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
	/// 今の時間を文字列として出力する
	/// </summary>
	/// <returns>今の時間(jst)</returns>
	std::string NowTime();
}