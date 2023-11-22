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