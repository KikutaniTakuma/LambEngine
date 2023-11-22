#include "WindowFactory.h"
#pragma comment(lib, "winmm.lib")
#include <cassert>
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WindowFactory::WindowFactory():
	hwnd_{},
	wndEx_{},
	windowStyle_(0u),
	windowRect_{},
	windowName_(),
	clientSize_{}
{
	timeBeginPeriod(1);
}

WindowFactory::~WindowFactory() {
	UnregisterClass(wndEx_.lpszClassName, wndEx_.hInstance);
}

LRESULT WindowFactory::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
	case WM_DESTROY:        // ウィンドウが破棄された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
}

void WindowFactory::Create(const std::wstring& windowTitle, int32_t width, int32_t height) {
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	windowName_ = windowTitle;

	// 最大化ボタンを持たないかつサイズ変更不可
	windowStyle_ = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

	wndEx_.cbSize = sizeof(WNDCLASSEX);
	wndEx_.lpfnWndProc = WindowProcedure;
	wndEx_.lpszClassName = windowTitle.c_str();
	wndEx_.hInstance = GetModuleHandle(nullptr);
	wndEx_.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndEx_);

	clientSize_ = {static_cast<float>(width),static_cast<float>(height)};

	windowRect_ = { 0,0,width, height };

	// 指定のサイズになるようなウィンドウサイズを計算
	AdjustWindowRect(&windowRect_, windowStyle_, false);

	hwnd_ = CreateWindow(
		wndEx_.lpszClassName,
		windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect_.right - windowRect_.left,
		windowRect_.bottom - windowRect_.top,
		nullptr,
		nullptr,
		wndEx_.hInstance,
		nullptr
	);

	windowStyle_ &= ~WS_THICKFRAME;

	SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);
	SetWindowPos(
		hwnd_, NULL, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED));
	ShowWindow(hwnd_, SW_NORMAL);
}


Vector2 WindowFactory::GetWindowSize() const {
	return Vector2(
		static_cast<float>(windowRect_.right), 
		static_cast<float>(windowRect_.bottom)
	);
}

Vector2 WindowFactory::GetClientSize() const {
	return clientSize_;
}