#pragma once

#include <Windows.h>
#undef min
#undef max
#include <stdint.h>
#include <string>
class Vector2;

/// <summary>
/// ウィンドウズAPI管理クラス(ウィンドウ生成等)
/// </summary>
class WinApp {
private:
	WinApp();
	~WinApp();
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;

public:
	static inline WinApp* GetInstance() {
		static WinApp instance;
		return &instance;
	}

	static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	void Create(const std::wstring& windowTitle, int32_t width, int32_t height);

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


private:
	HWND hwnd_ = nullptr;
	WNDCLASSEX wndEx_{};
	UINT windowStyle_;
	RECT windowRect_;
	std::wstring windowName_;
};