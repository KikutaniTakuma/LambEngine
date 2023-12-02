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
	void Create(const std::wstring& windowTitle, int32_t width, int32_t height);

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