#pragma once

#include <Windows.h>
#undef min
#undef max
#include <stdint.h>
#include <string>
#include "Utils/Math/Vector2.h"

/// <summary>
/// ウィンドウズAPI管理クラス(ウィンドウ生成等)
/// </summary>
class WindowFactory {
private:
	WindowFactory();
	~WindowFactory();
	WindowFactory(const WindowFactory&) = delete;
	const WindowFactory& operator=(const WindowFactory&) = delete;

public:
	static inline WindowFactory* GetInstance() {
		static WindowFactory instance;
		return &instance;
	}

public:
	static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	/// <summary>
	/// ウィンドウ生成
	/// </summary>
	/// <param name="windowTitle">ウィンドウ名</param>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="isFullscreen">フルスクリーン化</param>
	void Create(
		const std::wstring& windowTitle = L"LambEngine",
		int32_t width = 1280, 
		int32_t height = 720, 
		bool isFullscreen = false
	);

	void Fullscreen();

private:
	/// <summary>
	/// 引数がtrueならフルスクリーンに変更
	/// </summary>
	/// <param name="changeState">スクリーンの状態</param>
	void ChangeWindowMode();

public:

	inline HWND GetHwnd() const {
		return hwnd_;
	}

	inline const WNDCLASSEX& getWNDCLASSEX() const {
		return wndEx_;
	}

	inline std::wstring GetWindowClassName() const {
		return windowName_;
	}

	Vector2 GetWindowSize() const;

	Vector2 GetClientSize() const;


private:
	HWND hwnd_ = nullptr;
	WNDCLASSEX wndEx_{};
	UINT windowStyle_;
	RECT windowRect_;
	std::wstring windowName_;

	bool isFullscreen_;
	Vector2 clientSize_;
};