#pragma once
#include <Windows.h>

namespace UI {
	extern HWND MainWindow;
	extern HWND GraphicsArea;
	extern HWND Header;
	extern HWND StatusBar;
};

BOOL CreateUI(HINSTANCE hInstance);
